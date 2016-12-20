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
	m_startTime = av_gettime();
	m_formatContext = nullptr;
	m_codecContext = nullptr;
	av_register_all();
	avformat_network_init();
}

Pusher::~Pusher()
{
	if (!m_stopped)
	{
		StopPush();
	}
}

void Pusher::StartPush(std::string url, int width, int height, int frameRate)
{
	m_stopped = false;
	int ret = avformat_alloc_output_context2(&m_formatContext, nullptr, "flv", url.c_str()); //RTMP
	if (ret < 0) {
		throw ERROR_NO_OUTFMORTCONTEXT;
	}

	m_formatContext->oformat->video_codec = AV_CODEC_ID_H264;
	m_formatContext->oformat->audio_codec = AV_CODEC_ID_NONE;

	//Try find codec
	AVOutputFormat* oformat = m_formatContext->oformat;
	AVCodec* codec = avcodec_find_encoder(oformat->video_codec);
	if (codec == nullptr)
	{
		throw ERROR_NO_CODEC;
	}

	AVStream* outStream = avformat_new_stream(m_formatContext, codec);
	if (outStream == nullptr) 
	{
		throw ERROR_CREATE_OUTSTREAM;
	}

	m_codecContext = avcodec_alloc_context3(codec);
	if(m_codecContext == nullptr)
	{
		throw ERROR_CREATE_CODECCONTEXT;
	}

	m_codecContext->codec_id = oformat->video_codec;
	m_codecContext->width = width; //Width
	m_codecContext->height = height; //Height
	m_codecContext->time_base = {1,frameRate}; 
	m_codecContext->pix_fmt = AV_PIX_FMT_YUV420P; 

	if (oformat->flags & AVFMT_GLOBALHEADER)
		m_codecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;

	//Open codec
	if (avcodec_open2(m_codecContext, codec, nullptr) < 0)
	{
		throw ERROR_OPEN_CODEC;
	}
	//Copy the settings of AVCodecContext
	ret = avcodec_parameters_from_context(outStream->codecpar, m_codecContext);
	if (ret < 0) 
	{
		throw ERROR_COPY_CODEC_PARAM;
	}
	outStream->codecpar->codec_tag = 0;

	//Open output URL
	if (!(oformat->flags & AVFMT_NOFILE)) 
	{
		ret = avio_open(&m_formatContext->pb, url.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0) 
		{
			throw ERROR_OPEN_URL;
		}
	}

	//Write file header
	ret = avformat_write_header(m_formatContext, nullptr);
	if (ret < 0) 
	{
		throw ERROR_WRITEHEADER;
	}
	m_startTime = av_gettime();
}

void Pusher::StopPush()
{
	//Write file trailer
	if(av_write_trailer(m_formatContext) != 0)
	{
		throw ERROR_WRITETRAILER;
	}
	//close output
	if (m_formatContext != nullptr && !(m_formatContext->oformat->flags & AVFMT_NOFILE))
	{
		if(avio_close(m_formatContext->pb)<0)
		{
			throw ERROR_CLOSE_URL;
		}
	}
	avformat_free_context(m_formatContext);
	avcodec_close(m_codecContext);
	avcodec_free_context(&m_codecContext);

	m_formatContext = nullptr;
	m_codecContext = nullptr;

	m_startTime = av_gettime();
	m_stopped = true;
}

void Pusher::PushVideoPacket(VideoPacket* videoPacket)
{
	AVPacket* avPacket = videoPacket->GetPacket();

	//Delay if too fast, too fast may cause the server crash.
	AVRational time_base = m_codecContext->time_base;
	AVRational time_base_q = { 1,AV_TIME_BASE };
	int64_t pts_time = av_rescale_q(avPacket->dts, time_base, time_base_q);
	int64_t now_time = av_gettime() - m_startTime;
	if (pts_time > now_time)
		av_usleep(static_cast<unsigned int>(pts_time - now_time));

	//Re calc the pts dts and duration.
	AVStream* outStream = m_formatContext->streams[0];
	avPacket->pts = av_rescale_q_rnd(avPacket->pts, m_codecContext->time_base, outStream->time_base, static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	avPacket->dts = av_rescale_q_rnd(avPacket->dts, m_codecContext->time_base, outStream->time_base, static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	avPacket->duration = av_rescale_q(avPacket->duration, m_codecContext->time_base, outStream->time_base);

	AVPacket* clonedAVPacket = CloneAVPacket(avPacket);
	//Write the frame, notice that here must use av_interleaved_write_frame, it will automatic control the interval.
	//Notice2, after call av_interleaved_write_frame, the packet's data will be released.
	int ret = av_interleaved_write_frame(m_formatContext, clonedAVPacket);
	if (ret < 0)
	{
		throw ERROR_WRITE_FRAME;
	}
	av_packet_free(&clonedAVPacket);
}