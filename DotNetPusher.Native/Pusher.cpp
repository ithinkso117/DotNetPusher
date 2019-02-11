#include "stdafx.h"
#include "Pusher.h"
#include "ErrorCode.h"
#include "Utils.h"
#include <string>

extern "C"
{
#include "libavutil/time.h"
}

Pusher::Pusher()
{
	//Use av_log_set_level will make the application very slow.
	//av_log_set_level(AV_LOG_DEBUG);
	m_stopped = true;
	m_start_time = av_gettime();
	m_format_context = nullptr;
	m_codec_context = nullptr;
	av_register_all();
	avformat_network_init();
}

Pusher::~Pusher()
{
	if (!m_stopped)
	{
		stop_push();
	}
}

void Pusher::free_all()
{
	if (m_format_context != nullptr)
	{
		avformat_free_context(m_format_context);
		m_format_context = nullptr;
	}
	if (m_codec_context != nullptr)
	{
		avcodec_close(m_codec_context);
		avcodec_free_context(&m_codec_context);
		m_codec_context = nullptr;
	}
}

void Pusher::start_push(std::string url, int width, int height, int frame_rate)
{
	m_stopped = false;
	//RTMP is using flv format.
	auto ret = avformat_alloc_output_context2(&m_format_context, nullptr, "flv", url.c_str());
	if (ret < 0) 
	{
		Utils::write_log("Can not create output context.");
		throw ERROR_CREATE_PUSHER;
	}

	m_format_context->oformat->video_codec = AV_CODEC_ID_H264;
	m_format_context->oformat->audio_codec = AV_CODEC_ID_NONE;

	//Try find codec
	const auto format = m_format_context->oformat;
	const auto codec = avcodec_find_encoder(format->video_codec);
	if (codec == nullptr)
	{
		free_all();
		Utils::write_log("Can not find codec.");
		throw ERROR_CREATE_PUSHER;
	}

	const auto out_stream = avformat_new_stream(m_format_context, codec);
	if (out_stream == nullptr)
	{
		free_all();
		Utils::write_log("Can not create new output stream.");
		throw ERROR_CREATE_PUSHER;
	}

	m_codec_context = avcodec_alloc_context3(codec);
	if (m_codec_context == nullptr)
	{
		free_all();
		Utils::write_log("Can not create codec context.");
		throw ERROR_CREATE_PUSHER;
	}

	m_codec_context->codec_id = format->video_codec;
	m_codec_context->width = width; //Width
	m_codec_context->height = height; //Height
	m_codec_context->time_base = { 1,frame_rate };
	m_codec_context->pix_fmt = AV_PIX_FMT_YUV420P;

	if (format->flags & AVFMT_GLOBALHEADER)
		m_codec_context->flags |= CODEC_FLAG_GLOBAL_HEADER;

	////Open codec
	if (avcodec_open2(m_codec_context, codec, nullptr) < 0)
	{
		free_all();
		Utils::write_log("Can not open codec.");
		throw ERROR_CREATE_PUSHER;
	}
	//Copy the settings of AVCodecContext
	ret = avcodec_parameters_from_context(out_stream->codecpar, m_codec_context);
	if (ret < 0)
	{
		free_all();
		Utils::write_log("Can not copy parameters from codec context.");
		throw ERROR_CREATE_PUSHER;
	}
	out_stream->codecpar->codec_tag = 0;

	//Open output URL
	if (!(format->flags & AVFMT_NOFILE))
	{
		ret = avio_open(&m_format_context->pb, url.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0)
		{
			free_all();
			Utils::write_log("Open IO fail.");
			throw ERROR_CREATE_PUSHER;
		}
	}

	//Write file header
	ret = avformat_write_header(m_format_context, nullptr);
	if (ret < 0)
	{
		free_all();
		Utils::write_log("Write header error.");
		throw ERROR_CREATE_PUSHER;
	}
	m_start_time = av_gettime();
}

void Pusher::stop_push()
{
	//Write file trailer
	if (av_write_trailer(m_format_context) != 0)
	{
		free_all();
		Utils::write_log("Write trailer fail.");
		throw ERROR_STOP_PUSHER;
	}
	//close output
	if (m_format_context != nullptr && !(m_format_context->oformat->flags & AVFMT_NOFILE))
	{
		if (avio_close(m_format_context->pb) < 0)
		{
			free_all();
			Utils::write_log("Close IO error.");
			throw ERROR_STOP_PUSHER;
		}
	}
	free_all();
	m_start_time = av_gettime();
	m_stopped = true;
}

void Pusher::push_video_packet(VideoPacket* video_packet)
{
	const auto av_packet = video_packet->get_packet();

	////Delay if too fast, too fast may cause the server crash.
	const auto time_base = m_codec_context->time_base;
	const AVRational time_base_q = { 1,AV_TIME_BASE };
	const auto pts_time = av_rescale_q(av_packet->dts, time_base, time_base_q);
	const auto now_time = av_gettime() - m_start_time;
	if (pts_time > now_time)
	{
		av_usleep(static_cast<unsigned int>(pts_time - now_time));
		Utils::write_log("Frame sleep.");
	}

	//Re calc the pts dts and duration.
	const auto out_stream = m_format_context->streams[0];
	av_packet->pts = av_rescale_q_rnd(av_packet->pts, m_codec_context->time_base, out_stream->time_base, static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	av_packet->dts = av_rescale_q_rnd(av_packet->dts, m_codec_context->time_base, out_stream->time_base, static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	av_packet->duration = av_rescale_q(av_packet->duration, m_codec_context->time_base, out_stream->time_base);

	auto cloned_av_packet = Utils::clone_av_packet(av_packet);
	//Write the frame, notice that here must use av_interleaved_write_frame, it will automatic control the interval.
	//Notice2, after call av_interleaved_write_frame, the packet's data will be released.
	const int tick_count = GetTickCount();
	const auto ret = av_write_frame(m_format_context, cloned_av_packet);//av_interleaved_write_frame();
	if (ret < 0)
	{
		free_all();
		av_packet_free(&cloned_av_packet);
		cloned_av_packet = nullptr;
		throw ERROR_PUSH_PACKET;
	}
	const int time_used = GetTickCount() - tick_count;
	if (time_used > 300)
	{
		const auto str = new char[1024];
		sprintf_s(str, 1024, "Send frame too long. size:%d , %dms used.\n", cloned_av_packet->buf->size, time_used);
		Utils::write_log(str);
		delete[] str;
	}
	av_packet_free(&cloned_av_packet);
}
