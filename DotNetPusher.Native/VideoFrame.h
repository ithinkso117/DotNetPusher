#pragma once

extern "C"
{
#include "libavutil/frame.h"
}


class VideoFrame
{
	AVFrame* m_frame;
	int m_size;
	int m_index;
public:
	VideoFrame(AVFrame* frame, int index);
	~VideoFrame();
	int GetIndex();
	int GetSize();
	char* GetData();
};

