using System;
using System.Runtime.InteropServices;

namespace DotNetPusher.VideoPackets
{
    public partial class VideoPacket : IDisposable
    {
        private readonly IntPtr _packetHandle;
        private bool _disposed;

        public IntPtr Handle => _packetHandle;

        public int Index { get; }

        public int Size { get; }

        public byte[] Data
        {
            get
            {
                IntPtr dataPtr = Environment.Is64BitProcess ?
                     Interop64.GetPacketData(_packetHandle) :
                     Interop32.GetPacketData(_packetHandle);
                var data = new byte[Size];
                Marshal.Copy(dataPtr, data, 0, Size);
                return data;
            }
        }

        public VideoPacket(IntPtr packetHandle)
        {
            _packetHandle = packetHandle;
            Index = Environment.Is64BitProcess ?
                Interop64.GetPacketIndex(_packetHandle) :
                Interop32.GetPacketIndex(_packetHandle);

            Size = Environment.Is64BitProcess ?
                Interop64.GetPacketSize(_packetHandle) :
                Interop32.GetPacketSize(_packetHandle);
        }

        ~VideoPacket()
        {
            DoDispose();
        }

        private void DoDispose()
        {
            if (!_disposed)
            {
                var result = Environment.Is64BitProcess ?
                    Interop64.DestroyPacket(_packetHandle) :
                    Interop32.DestroyPacket(_packetHandle);
                if (result != 0)
                {
                    throw new PusherException(result);
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
