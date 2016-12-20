// MediaUtil.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "NativePusher.h"
#include "Encoder.h"
#include "Pusher.h"
#include "ErrorCode.h"
#include "VideoFrame.h"


int __stdcall CreateEncoder(void** hEncoder, int width, int height, int frameRate, void* frameEncodedCallback)
{
	try
	{
		void* result = static_cast<void*>(new Encoder(width, height, frameRate, static_cast<FrameEncodedCallback>(frameEncodedCallback)));
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

int __stdcall AddImage(void* hEncoder, char* imageData, int dataLength)
{
	Encoder* encoder = static_cast<Encoder*>(hEncoder);
	try
	{
		encoder->AddImage(imageData, dataLength);
	}
	catch (ErrorCode e)
	{
		return e;
	}
	return 0;
}

int __stdcall FlushEncoder(void* hEncoder)
{
	Encoder* encoder = static_cast<Encoder*>(hEncoder);
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
		void* result = static_cast<void*>(new Pusher());
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
	Pusher* pusher = static_cast<Pusher*>(hPusher);
	try
	{
		pusher->StartPush(url, width, height, frameRate);
	}
	catch (ErrorCode e)
	{
		return e;
	}
	return 0;
}

int __stdcall StopPush(void* hPusher)
{
	Pusher* pusher = static_cast<Pusher*>(hPusher);
	try
	{
		pusher->StopPush();
	}
	catch (ErrorCode e)
	{
		return e;
	}
	return 0;
}

int __stdcall PushPacket(void* hPusher, void* hPacket)
{
	Pusher* pusher = static_cast<Pusher*>(hPusher);
	VideoPacket* videoPacket = static_cast<VideoPacket*>(hPacket);
	try
	{
		pusher->PushVideoPacket(videoPacket);
	}
	catch (ErrorCode e)
	{
		return e;
	}
	return 0;
}



int __stdcall GetFrameIndex(void* hFrame)
{
	VideoFrame* videoFrame = static_cast<VideoFrame*>(hFrame);
	return videoFrame->GetIndex();
}

int __stdcall GetFrameSize(void* hFrame)
{
	VideoFrame* videoFrame = static_cast<VideoFrame*>(hFrame);
	return videoFrame->GetSize();
}


char* __stdcall GetFrameData(void* hFrame)
{
	VideoFrame* videoFrame = static_cast<VideoFrame*>(hFrame);
	return videoFrame->GetData();
}

void __stdcall DestroyFrame(void* hFrame)
{
	VideoFrame* videoFrame = static_cast<VideoFrame*>(hFrame);
    delete videoFrame;
}


int __stdcall GetPacketIndex(void* hPacket)
{
	VideoPacket* videoPacket = static_cast<VideoPacket*>(hPacket);
	return videoPacket->GetIndex();
}

int __stdcall GetPacketSize(void* hPacket)
{
	VideoPacket* videoPacket = static_cast<VideoPacket*>(hPacket);
	return videoPacket->GetSize();
}


char* __stdcall GetPacketData(void* hPacket)
{
	VideoPacket* videoPacket = static_cast<VideoPacket*>(hPacket);
	return videoPacket->GetData();
}

int __stdcall DestroyPacket(void* hPacket)
{
	VideoPacket* videoPacket = static_cast<VideoPacket*>(hPacket);
	try
	{
		if(videoPacket->GetReferenced())
		{
			throw ERROR_PACKET_REFERENCED;
		}
		delete videoPacket;
	}
	catch (ErrorCode e)
	{
		return e;
	}
	return 0;
}



