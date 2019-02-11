using System;
using DotNetPusher.VideoPackets;

namespace DotNetPusher.Pushers
{
    public partial class Pusher
    {
        private readonly IntPtr _pusherHandle;
        private bool _disposed;

        public Pusher()
        {
            var result = Environment.Is64BitProcess ?
                  Interop64.CreatePusher(out _pusherHandle) :
                  Interop32.CreatePusher(out _pusherHandle);
            if (result != 0)
            {
                throw new PusherException(result);
            }
        }

        ~Pusher()
        {
            DoDispose();
        }

        public void StartPush(string url, int width, int height, int frameRate)
        {
            var result = Environment.Is64BitProcess ?
                Interop64.StartPush(_pusherHandle, url, width, height, frameRate) :
                Interop32.StartPush(_pusherHandle, url, width, height, frameRate);
            if (result != 0)
            {
                throw new PusherException(result);
            }
        }

        public void StopPush()
        {
            var result = Environment.Is64BitProcess ?
                Interop64.StopPush(_pusherHandle) :
                Interop32.StopPush(_pusherHandle);
            if (result != 0)
            {
                throw new PusherException(result);
            }
        }

        public void PushPacket(VideoPacket packet)
        {
            var result = Environment.Is64BitProcess ?
                Interop64.PushPacket(_pusherHandle, packet.Handle) :
                Interop32.PushPacket(_pusherHandle, packet.Handle);
            if (result != 0)
            {
                throw new PusherException(result);
            }
        }

        private void DoDispose()
        {
            if (!_disposed)
            {
                if (Environment.Is64BitProcess)
                {
                    Interop64.DestroyPusher(_pusherHandle);
                }
                else
                {
                    Interop32.DestroyPusher(_pusherHandle);
                }
                _disposed = true;
            }
        }

        public void Dispose()
        {
            DoDispose();
            GC.SuppressFinalize(this);
        }
    }
}
