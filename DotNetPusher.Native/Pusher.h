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
	AVFormatContext* m_formatContext;
	AVCodecContext* m_codecContext;
	int64_t m_startTime;
	bool m_stopped;
public:
	Pusher();
	~Pusher();
	void StartPush(std::string url, int width, int height, int frameRate);
	void StopPush();
	void PushVideoPacket(VideoPacket* videoPacket);
};
