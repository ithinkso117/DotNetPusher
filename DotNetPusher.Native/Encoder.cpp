#include "stdafx.h"
#include "Encoder.h"
#include "ErrorCode.h"
#include "Utils.h"

extern "C" 
{
#include "libavutil/frame.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
}


Encoder::Encoder(int width, int height, int frameRate, FrameEncodedCallback frameEncodedCallback)
{
	//Use av_log_set_level will make the application very slow.
	//av_log_set_level(AV_LOG_DEBUG);
	av_register_all();
	avcodec_register_all();

	m_width = width;
	m_height = height;
	m_encodedIndex = 0;
	m_frameEncoded = frameEncodedCallback;

	//Try find H264 codec
	AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (codec == nullptr)
	{
		throw ERROR_NO_CODEC;
	}
	m_codecContext = avcodec_alloc_context3(codec);
	m_codecContext->bit_rate = 1024*1024;// put sample parameters   
	m_codecContext->width = m_width; //Width
	m_codecContext->height = m_height; //Height
	m_codecContext->time_base = {1,frameRate};  //frames per second
	m_codecContext->gop_size = 10; // emit one intra frame every ten frames   
	m_codecContext->max_b_frames = 1; //B frames
	m_codecContext->thread_count = 1; //Thread used.
	m_codecContext->pix_fmt = AV_PIX_FMT_YUV420P; //YUV420 for H264

	//Set parameters
	AVDictionary *options = nullptr;
	av_dict_set(&options, "profile", "baseline", 0);
	av_dict_set(&options, "preset", "fast", 0);

	//Open H264 codec
	if (avcodec_open2(m_codecContext, codec, &options) < 0)
	{
		throw ERROR_OPEN_CODEC;
	}

	m_yuvFrame = av_frame_alloc();
	m_yuvFrame->pts = 0;
	m_yuvFrame->format = AV_PIX_FMT_YUV420P;
	m_yuvFrame->width = m_codecContext->width;
	m_yuvFrame->height = m_codecContext->height;
	
	m_swsContext = sws_getContext(m_codecContext->width, m_codecContext->height, AV_PIX_FMT_BGRA, m_codecContext->width, m_codecContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);
}

Encoder::~Encoder()
{
	m_frameEncoded = nullptr;
	av_frame_free(&m_yuvFrame);
	avcodec_free_context(&m_codecContext);
	sws_freeContext(m_swsContext);
}

void Encoder::Flush()
{
	//Flush encoder
	if (avcodec_send_frame(m_codecContext, nullptr) == 0)
	{
		int eof = AVERROR(AVERROR_EOF);
		while (1)
		{
			AVPacket* avPacket = CreateAVPacket();
			int ret = avcodec_receive_packet(m_codecContext, avPacket);
			if (ret == eof || ret < 0)
			{
				av_packet_free(&avPacket);
				break;
			}
			VideoPacket* videoPacket = new VideoPacket(avPacket, m_encodedIndex);
			if (m_frameEncoded != nullptr)
			{
				m_frameEncoded(this,videoPacket);
			}
			m_encodedIndex++;

		}
	}
}


void  Encoder::AddImage(char* imageData, int dataLength)
{
	uint8_t* bgraBuffer = new uint8_t[dataLength]; //bgra buffer
	uint8_t* yuvBuffer = new uint8_t[(m_width * m_height * 3) / 2]; // size for YUV 420  
	memcpy(bgraBuffer, imageData, dataLength);
 

	AVFrame* bgraFrame = av_frame_alloc();
	bgraFrame->format = AV_PIX_FMT_BGRA;
	bgraFrame->width = m_width;
	bgraFrame->height = m_height;

	//Fill rgb frame
	av_image_fill_arrays(bgraFrame->data, bgraFrame->linesize, bgraBuffer, AV_PIX_FMT_BGRA, m_width, m_height, 1);

	//Fill yuv Frame  
	av_image_fill_arrays(m_yuvFrame->data, m_yuvFrame->linesize, yuvBuffer, AV_PIX_FMT_YUV420P, m_width, m_height, 1);

	//Convert ARGB to YUV
	sws_scale(m_swsContext, bgraFrame->data, bgraFrame->linesize, 0, m_height, m_yuvFrame->data, m_yuvFrame->linesize);

	m_yuvFrame->pts++;
	if (avcodec_send_frame(m_codecContext, m_yuvFrame) == 0)
	{	
		while (1)
		{
			AVPacket* avPacket = CreateAVPacket();
			int ret = avcodec_receive_packet(m_codecContext, avPacket);
			if(ret < 0)
			{
				av_packet_free(&avPacket);
				break;
			}
			VideoPacket* videoPacket = new VideoPacket(avPacket, m_encodedIndex);
			if (m_frameEncoded != nullptr)
			{
				m_frameEncoded(this,videoPacket);
			}
			m_encodedIndex++;
		}
	}
	delete[] yuvBuffer;
	delete[] bgraBuffer;
	av_frame_free(&bgraFrame);
}
