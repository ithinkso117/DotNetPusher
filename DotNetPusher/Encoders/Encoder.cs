using System;
using DotNetPusher.VideoPackets;

namespace DotNetPusher.Encoders
{
    public partial class Encoder:IDisposable
    {
        public delegate void FrameEncodedCallback(IntPtr hDecoder, IntPtr hPacket);

        private readonly IntPtr _encoderHandle;
        private FrameEncodedCallback _myFrameEncodedCallback;
        private bool _disposed;


        public event EventHandler<FrameEncodedEventArgs> FrameEncoded;

        public IntPtr Handle => _encoderHandle;

        public Encoder(int width, int height, int frameRate)
        {
            _myFrameEncodedCallback = MyFrameEncodedCallback;
            var result = Environment.Is64BitProcess ? 
                  Interop64.CreateEncoder(out _encoderHandle, width, height, frameRate, _myFrameEncodedCallback) : 
                  Interop32.CreateEncoder(out _encoderHandle, width, height, frameRate, _myFrameEncodedCallback);
            if (result != 0)
            {
                throw new PusherException(result);
            }
        }

        ~Encoder()
        {
            DoDispose();
        }

        private void MyFrameEncodedCallback(IntPtr hEncoder, IntPtr hPacket)
        {
            if (hEncoder == _encoderHandle)
            {
                var videoPacket = new VideoPacket(hPacket);
                OnFrameEncoded(new FrameEncodedEventArgs(videoPacket));
            }
        }

        public void Flush()
        {
            var result = Environment.Is64BitProcess ?
                Interop64.FlushEncoder(_encoderHandle) :
                Interop32.FlushEncoder(_encoderHandle);
            if (result != 0)
            {
                throw new PusherException(result);
            }
        }

        /// <summary>
        /// Only support BGRA data.
        /// </summary>
        /// <param name="imageData"></param>
        /// <returns></returns>
        public void AddImage(byte[] imageData)
        {
            var result = Environment.Is64BitProcess ?
                Interop64.AddImage(_encoderHandle, imageData, imageData.Length):
                Interop32.AddImage(_encoderHandle, imageData, imageData.Length);
            if (result != 0)
            {
                throw new PusherException(result);
            }
        }

        private void DoDispose()
        {
            if (!_disposed)
            {
                _myFrameEncodedCallback = null;
                if (Environment.Is64BitProcess)
                {
                    Interop64.DestroyEncoder(_encoderHandle);
                }
                else
                {
                   Interop32.DestroyEncoder(_encoderHandle);
                }
                _disposed = true;
            }
        }

        public void Dispose()
        {
            DoDispose();
            GC.SuppressFinalize(this);
        }

        private void OnFrameEncoded(FrameEncodedEventArgs e)
        {
            FrameEncoded?.Invoke(this, e);
        }
    }
}
