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
	VideoPacket(AVPacket* avPacket, int index);
	~VideoPacket();
	int GetIndex();
	int GetSize();
	char* GetData();
	AVPacket* GetPacket();
	void SetReferenced(bool value);
	bool GetReferenced();
};
