// MediaUtil.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "NativePusher.h"
#include "Encoder.h"
#include "Pusher.h"
#include "ErrorCode.h"
#include "VideoFrame.h"


int __stdcall CreateEncoder(void** hEncoder, int width, int height, int frame_rate, int bit_rate, void* frameEncodedCallback)
{
	try
	{
		const auto result = static_cast<void*>(new Encoder(width, height, frame_rate, bit_rate, static_cast<FrameEncodedCallback>(frameEncodedCallback)));
		*hEncoder = result;
	}
	catch(ErrorCode e)
	{
		*hEncoder = nullptr;
		return e;
	}
	return 0;
}

void __stdcall DestroyEncoder(void* hEncoder)
{
	delete static_cast<Encoder*>(hEncoder);
}

int __stdcall AddImage(void* hEncoder, uint8_t* image_data)
{
	auto encoder = static_cast<Encoder*>(hEncoder);
	try
	{
		encoder->AddImage(image_data);
	}
	catch (ErrorCode e)
	{
		return e;
	}
	return 0;
}

int __stdcall FlushEncoder(void* hEncoder)
{
	auto encoder = static_cast<Encoder*>(hEncoder);
	try
	{
		encoder->Flush();
	}
	catch (ErrorCode e)
	{
		return e;
	}
	return 0;
}



int __stdcall CreatePusher(void** hPusher)
{
	try
	{
		const auto result = static_cast<void*>(new Pusher());
		*hPusher = result;
	}
	catch (ErrorCode e)
	{
		*hPusher = nullptr;
		return e;
	}
	return 0;
}

void __stdcall DestroyPusher(void* hPusher)
{
	delete static_cast<Pusher*>(hPusher);
}


int __stdcall StartPush(void* hPusher, const char* url, int width, int height, int frameRate)
{
	auto pusher = static_cast<Pusher*>(hPusher);
	try
	{
		pusher->start_push(url, width, height, frameRate);
	}
	catch (ErrorCode e)
	{
		return e;
	}
	return 0;
}

int __stdcall StopPush(void* hPusher)
{
	auto pusher = static_cast<Pusher*>(hPusher);
	try
	{
		pusher->stop_push();
	}
	catch (ErrorCode e)
	{
		return e;
	}
	return 0;
}

int __stdcall PushPacket(void* hPusher, void* hPacket)
{
	auto pusher = static_cast<Pusher*>(hPusher);
	const auto video_packet = static_cast<VideoPacket*>(hPacket);
	try
	{
		pusher->push_video_packet(video_packet);
	}
	catch (ErrorCode e)
	{
		return e;
	}
	return 0;
}



int __stdcall GetFrameIndex(void* hFrame)
{
	auto video_frame = static_cast<VideoFrame*>(hFrame);
	return video_frame->get_index();
}

int __stdcall GetFrameSize(void* hFrame)
{
	VideoFrame* video_frame = static_cast<VideoFrame*>(hFrame);
	return video_frame->get_size();
}


char* __stdcall GetFrameData(void* hFrame)
{
	auto video_frame = static_cast<VideoFrame*>(hFrame);
	return video_frame->get_data();
}

void __stdcall DestroyFrame(void* hFrame)
{
	const auto video_frame = static_cast<VideoFrame*>(hFrame);
    delete video_frame;
}


int __stdcall GetPacketIndex(void* hPacket)
{
	const auto video_packet = static_cast<VideoPacket*>(hPacket);
	return video_packet->get_index();
}

int __stdcall GetPacketSize(void* hPacket)
{
	const auto video_packet = static_cast<VideoPacket*>(hPacket);
	return video_packet->get_size();
}


char* __stdcall GetPacketData(void* hPacket)
{
	const auto video_packet = static_cast<VideoPacket*>(hPacket);
	return video_packet->get_data();
}

int __stdcall DestroyPacket(void* hPacket)
{
	const auto video_packet = static_cast<VideoPacket*>(hPacket);
	try
	{
		if(video_packet->get_referenced())
		{
			throw ERROR_PACKET_REFERENCED;
		}
		delete video_packet;
	}
	catch (ErrorCode e)
	{
		return e;
	}
	return 0;
}



