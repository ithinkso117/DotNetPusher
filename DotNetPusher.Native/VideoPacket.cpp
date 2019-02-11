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
	m_packet = nullptr;
}

int VideoPacket::get_index() const
{
	return m_index;
}

int VideoPacket::get_size() const
{
	return m_packet->size;
}

char* VideoPacket::get_data() const
{
	return reinterpret_cast<char*>(m_packet->data);
}

AVPacket* VideoPacket::get_packet() const
{
	return m_packet;
}

void VideoPacket::set_referenced(bool value)
{
	m_referenced = value;
}

bool VideoPacket::get_referenced() const
{
	return m_referenced;
}
