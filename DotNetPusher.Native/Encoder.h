#pragma once

#include "VideoPacket.h"

extern "C"
{
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
}

class VideoPacket;

typedef void(__stdcall *FrameEncodedCallback)(void* encoder, VideoPacket* videoPacket);

class Encoder
{
private:
	SwsContext* m_swsContext;
	AVCodecContext* m_codecContext;
	AVFrame* m_yuvFrame;
	int m_encodedIndex;
	int m_width;
	int m_height;
	FrameEncodedCallback m_frameEncoded;
public:
	Encoder(int width, int height, int frameRate, FrameEncodedCallback frameEncodedCallback);
	~Encoder();
	void AddImage(char* imageData, int dataLength);
	void Flush();
};