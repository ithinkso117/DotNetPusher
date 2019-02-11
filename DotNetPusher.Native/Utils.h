#pragma once

extern "C"
{
#include "libavcodec/avcodec.h"
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

typedef void(__stdcall *LogCallback)(char* log, int size);

static LogCallback g_log_callback = nullptr;

class Utils
{
public:
	static AVPacket* create_av_packet();
	static AVPacket* clone_av_packet(AVPacket* av_packet);
	static int fill_frame(AVFrame* frame, uint8_t* frame_data);
	static void convert_frame(SwsContext* sc, AVFrame* source, AVFrame* target);
	static void write_log(char* log);
};


inline AVPacket* Utils::create_av_packet()
{
	const auto av_packet = av_packet_alloc();
	av_init_packet(av_packet);
	av_packet->data = nullptr;
	av_packet->size = 0;
	return av_packet;
}

inline AVPacket* Utils::clone_av_packet(AVPacket* av_packet)
{
	const auto cloned_av_packet = create_av_packet();
	av_packet_copy_props(cloned_av_packet, av_packet);
	av_packet_ref(cloned_av_packet, av_packet);
	return cloned_av_packet;
}

inline int Utils::fill_frame(AVFrame* frame, uint8_t* frame_data)
{
	return av_image_fill_arrays(frame->data, frame->linesize, frame_data, static_cast<AVPixelFormat>(frame->format), frame->width, frame->height, 1);
}

inline void Utils::convert_frame(SwsContext* sc, AVFrame* source, AVFrame* target)
{
	sws_scale(sc, source->data, source->linesize, 0, source->height, target->data, target->linesize);
}

inline void Utils::write_log(char* log)
{
	if (g_log_callback != nullptr)
	{
		const auto size = static_cast<int>(strlen(log));
		g_log_callback(log, size);
	}
	else
	{
		printf("%s\n",log);
	}
}