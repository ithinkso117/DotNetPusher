#pragma once

extern "C"
{
#include "libavcodec/avcodec.h"
}


class VideoPacket
{
	AVPacket* m_packet;
	bool m_referenced;
	int m_index;
public:
	VideoPacket(AVPacket* av_packet, int index);
	~VideoPacket();
	int get_index() const;
	int get_size() const;
	char* get_data() const;
	AVPacket* get_packet() const;
	void set_referenced(bool value);
	bool get_referenced() const;
};
