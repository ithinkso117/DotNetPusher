using System;
using System.Runtime.InteropServices;

namespace DotNetPusher.VideoFrames
{
    public sealed partial class VideoFrame:IDisposable
    {
        private readonly IntPtr _frameHandle;
        private bool _disposed;

        public int Index { get; }

        public int Size { get; }

        public byte[] Data
        {
            get
            {
                IntPtr dataPtr = Environment.Is64BitProcess ?
                    Interop64.GetFrameData(_frameHandle) :
                    Interop32.GetFrameData(_frameHandle);
                var data = new byte[Size];
                Marshal.Copy(dataPtr,data,0,Size);
                return data;
            }
        }


        // ReSharper disable once UnusedMember.Local
        private VideoFrame()
        {
            //Hide the default constructor.
        }

        internal VideoFrame(IntPtr frameHandle)
        {
            _frameHandle = frameHandle;
            Index = Environment.Is64BitProcess ?
                Interop64.GetFrameIndex(_frameHandle) :
                Interop32.GetFrameIndex(_frameHandle);

            Size = Environment.Is64BitProcess ?
                Interop64.GetFrameSize(_frameHandle) :
                Interop32.GetFrameSize(_frameHandle);
        }

        ~VideoFrame()
        {
            DoDispose();
        }

        private void DoDispose()
        {
            if (!_disposed)
            {
                if (Environment.Is64BitProcess)
                {
                    Interop64.DestroyFrame(_frameHandle);
                }
                else
                {
                    Interop32.DestroyFrame(_frameHandle);
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
