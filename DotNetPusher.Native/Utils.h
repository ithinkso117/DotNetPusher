#pragma once

extern "C"
{
#include "libavcodec/avcodec.h"
}


inline AVPacket* CreateAVPacket()
{
	AVPacket* avPacket = av_packet_alloc();
	av_init_packet(avPacket);
	avPacket->data = nullptr;
	avPacket->size = 0;
	return avPacket;
}

inline AVPacket* CloneAVPacket(AVPacket* avPacket)
{
	AVPacket* clonedAvPacket = CreateAVPacket();
	av_packet_copy_props(clonedAvPacket, avPacket);
	av_packet_ref(clonedAvPacket, avPacket);
	return clonedAvPacket;
}