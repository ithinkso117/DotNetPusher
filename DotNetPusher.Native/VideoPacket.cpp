#include "stdafx.h"
#include "VideoPacket.h"


VideoPacket::VideoPacket(AVPacket* avPacket, int index)
{
	m_index = index;
	m_referenced = false;
	m_packet = avPacket;
}

VideoPacket::~VideoPacket()
{
	av_packet_free(&m_packet);
}

int VideoPacket::GetIndex()
{
	return m_index;
}

int VideoPacket::GetSize()
{
	return m_packet->size;
}

char* VideoPacket::GetData()
{
	return reinterpret_cast<char*>(m_packet->data);
}

AVPacket* VideoPacket::GetPacket()
{
	return m_packet;
}

void VideoPacket::SetReferenced(bool value)
{
	m_referenced = value;
}

bool VideoPacket::GetReferenced()
{
	return m_referenced;
}