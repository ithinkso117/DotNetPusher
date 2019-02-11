#pragma once
#include "VideoPacket.h"
#include <string>

class VideoPacket;

extern "C"
{
#include "libavformat/avformat.h"
}


class Pusher
{
	AVFormatContext* m_format_context{};
	AVCodecContext* m_codec_context;
	int64_t m_start_time{};
	bool m_stopped;
	void free_all();
public:
	Pusher();
	~Pusher();
	void start_push(std::string url, int width, int height, int frame_rate);
	void stop_push();
	void push_video_packet(VideoPacket* video_packet);
};
