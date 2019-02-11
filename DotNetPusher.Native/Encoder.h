#pragma once

#include "VideoPacket.h"

extern "C"
{
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
}

class VideoPacket;

typedef void(__stdcall *FrameEncodedCallback)(void* encoder, VideoPacket* video_packet);

class Encoder final
{
	SwsContext* m_sws_context{};
	AVCodecContext* m_codec_context;
	AVFrame* m_yuv_frame{};
	AVFrame* m_bgra_frame{};
	int m_encoded_index;
	int m_width;
	int m_height;
	int m_frame_rate;
	FrameEncodedCallback m_frame_encoded_callback;
	void encode_frame(bool flush);
	void free_all();
	void init_h264_context(int width, int height, int frame_rate, int bit_rate);
public:
	Encoder(int width, int height, int frame_rate, int bit_rate, FrameEncodedCallback frame_encoded_callback);
	~Encoder();
	void AddImage(uint8_t* image_data);
	void Flush();
};
