#pragma once
#include <cstdint>

extern "C" {

_declspec(dllexport) int __stdcall CreateEncoder(void** hEncoder, int width, int height, int frame_rate, int bit_rate, void* frameEncodedCallback);
_declspec(dllexport) void __stdcall DestroyEncoder(void* hEncoder);
_declspec(dllexport) int __stdcall AddImage(void* hEncoder, uint8_t* image_data);
_declspec(dllexport) int __stdcall FlushEncoder(void* hEncoder);

_declspec(dllexport) int __stdcall CreatePusher(void** hPusher);
_declspec(dllexport) void __stdcall DestroyPusher(void* hPusher);
_declspec(dllexport) int __stdcall StartPush(void* hPusher, const char* url, int width, int height, int frameRate);
_declspec(dllexport) int __stdcall StopPush(void* hPusher);
_declspec(dllexport) int __stdcall PushPacket(void* hPusher, void* hPacket);


_declspec(dllexport) int __stdcall GetFrameIndex(void* hFrame);
_declspec(dllexport) int __stdcall GetFrameSize(void* hFrame);
_declspec(dllexport) char* __stdcall GetFrameData(void* hFrame);
_declspec(dllexport) void __stdcall DestroyFrame(void* hFrame);;

_declspec(dllexport) int __stdcall GetPacketIndex(void* hPacket);
_declspec(dllexport) int __stdcall GetPacketSize(void* hPacket);
_declspec(dllexport) char* __stdcall GetPacketData(void* hPacket);
_declspec(dllexport) int __stdcall DestroyPacket(void* hPacket);






}






