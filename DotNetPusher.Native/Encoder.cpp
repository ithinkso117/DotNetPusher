#include "stdafx.h"
#include "Encoder.h"
#include "ErrorCode.h"
#include "Utils.h"

extern "C" 
{
#include "libavutil/frame.h"
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
}


Encoder::Encoder(int width, int height, int frame_rate, int bit_rate, FrameEncodedCallback frame_encoded_callback)
{
	m_sws_context = nullptr;
	m_bgra_frame = nullptr;
	m_yuv_frame = nullptr;
	m_codec_context = nullptr;
	m_width = width;
	m_height = height;
	m_frame_rate = frame_rate;
	m_encoded_index = 0;
	m_frame_encoded_callback = frame_encoded_callback;

	//Use av_log_set_level will make the application very slow.
	//av_log_set_level(AV_LOG_DEBUG);
	av_register_all();
	avcodec_register_all();
	
    init_h264_context(m_width, m_height, m_frame_rate, bit_rate);
	if (m_codec_context == nullptr)
	{
		Utils::write_log("Can not create CodecContext.");
		throw ERROR_CREATE_ENCODER;
	}

	m_yuv_frame = av_frame_alloc();
	if (m_yuv_frame == nullptr)
	{
		Utils::write_log("Can not create Frame.");
		free_all();
		throw ERROR_CREATE_ENCODER;
	}
	m_yuv_frame->pts = 0;
	m_yuv_frame->format = m_codec_context->pix_fmt;
	m_yuv_frame->width = m_width;
	m_yuv_frame->height = m_height;

	//Fill the yuv frame.
	const auto yuv_frame_data_length = m_width * m_height * 3 / 2;
	const auto yuv_frame_data = new uint8_t[yuv_frame_data_length];
	memset(yuv_frame_data, 0, yuv_frame_data_length);
	if (Utils::fill_frame(m_yuv_frame, yuv_frame_data) < 0)
	{
		Utils::write_log("init yuv frame fail.");
		throw ERROR_CREATE_ENCODER;
	}

	m_bgra_frame = av_frame_alloc();
	if (m_bgra_frame == nullptr)
	{
		Utils::write_log("Can not create BGRA frame.");
		free_all();
		throw ERROR_CREATE_ENCODER;
	}
	m_bgra_frame->format = AV_PIX_FMT_BGRA;
	m_bgra_frame->width = m_width;
	m_bgra_frame->height = m_height;

	m_sws_context = sws_getContext(m_codec_context->width, m_codec_context->height, AV_PIX_FMT_BGRA, m_codec_context->width, m_codec_context->height, m_codec_context->pix_fmt, SWS_BICUBIC, nullptr, nullptr, nullptr);
	if (m_sws_context == nullptr)
	{
		Utils::write_log("Can not create SwsContext.");
		free_all();
		throw ERROR_CREATE_ENCODER;
	}
	Utils::write_log("Encoder initialized.");
}


Encoder::~Encoder()
{
	free_all();
}


void Encoder::encode_frame(bool flush)
{
	if (avcodec_send_frame(m_codec_context, flush ? nullptr : m_yuv_frame) == 0)
	{
		const auto eof = AVERROR(AVERROR_EOF);
		while (true)
		{
			auto av_packet = Utils::create_av_packet();
			const auto ret = avcodec_receive_packet(m_codec_context, av_packet);
			if (ret == eof || ret < 0)
			{
				av_packet_free(&av_packet);
				av_packet = nullptr;
				break;
			}
			const auto video_packet = new VideoPacket(av_packet, m_encoded_index);
			if (m_frame_encoded_callback != nullptr)
			{
				m_frame_encoded_callback(this, video_packet);
			}
			else
			{
				delete video_packet;
			}
			m_encoded_index++;
		}
	}
}

void Encoder::free_all()
{
	m_frame_encoded_callback = nullptr;
	if (m_yuv_frame != nullptr)
	{
		av_frame_free(&m_yuv_frame);
		m_yuv_frame = nullptr;
	}
	if (m_bgra_frame != nullptr)
	{
		av_frame_free(&m_bgra_frame);
		m_bgra_frame = nullptr;
	}
	if (m_sws_context != nullptr)
	{
		sws_freeContext(m_sws_context);
		m_sws_context = nullptr;
	}
	if (m_codec_context != nullptr)
	{
		avcodec_free_context(&m_codec_context);
		m_codec_context = nullptr;
	}
}


void Encoder::init_h264_context(int width, int height, int frame_rate, int bit_rate)
{
	const auto codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (codec == nullptr)
	{
		return;
	}
	auto codec_context = avcodec_alloc_context3(codec);
	if (codec_context == nullptr)
	{
		return;
	}

	codec_context->width = width; //Width
	codec_context->height = height; //Height
	codec_context->time_base = { 1, frame_rate };  //Frames per second
	codec_context->thread_count = 1; //Use single thread.
	codec_context->pix_fmt = *codec->pix_fmts;//AV_PIX_FMT_YUV420P; //YUV420 for H264
	codec_context->bit_rate = bit_rate;// Put sample parameters   
	codec_context->gop_size = frame_rate; // Set gop size to the frame rate.   
	codec_context->max_b_frames = 1; //B frames

	//Set parameters
	AVDictionary *options = nullptr;
	av_dict_set(&options, "profile", "baseline", 0);
	av_dict_set(&options, "tune", "zerolatency", 0);
	av_dict_set(&options, "preset", "superfast", 0);

	if (avcodec_open2(codec_context, codec, &options) < 0)
	{
		av_dict_free(&options);
		avcodec_free_context(&codec_context);
		return;
	}
	m_codec_context = codec_context;
}


void Encoder::Flush()
{
	encode_frame(true);
}


void  Encoder::AddImage(uint8_t* image_data)
{
	//Fill rgb frame
	if (Utils::fill_frame(m_bgra_frame, image_data) < 0)
	{
		Utils::write_log("Fill BGRA frame fail.");
		throw ERROR_ENCODE;
	}

	//Convert bgra to yuv
	Utils::convert_frame(m_sws_context, m_bgra_frame, m_yuv_frame);
	m_yuv_frame->pts++;
	encode_frame(false);
}
