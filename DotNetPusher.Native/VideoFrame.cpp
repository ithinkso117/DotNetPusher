#include "stdafx.h"
#include "VideoFrame.h"


VideoFrame::VideoFrame(AVFrame* frame, int index)
{
	m_index = index;
	m_frame = frame;
	m_size = m_frame->width* m_frame->height * 4;
}

VideoFrame::~VideoFrame()
{
	av_frame_free(&m_frame);
}

int VideoFrame::GetIndex()
{
	return m_index;
}

int VideoFrame::GetSize()
{
	return m_size;
}

char* VideoFrame::GetData()
{
	return reinterpret_cast<char*>(m_frame->data[0]);
}