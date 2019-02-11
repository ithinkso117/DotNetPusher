using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using DotNetPusher.VideoPackets;

namespace DotNetPusher.Encoders
{
    public partial class Encoder:IDisposable
    {
        public delegate void FrameEncodedCallback(IntPtr hDecoder, IntPtr hPacket);

        private readonly IntPtr _encoderHandle;
        private FrameEncodedCallback _myFrameEncodedCallback;
        private bool _disposed;

        private readonly Bitmap _layerImage;
        private readonly Graphics _layerGraphics;
        private readonly Rectangle _layerRect;
        private readonly int _layerDataSzie;

        public event EventHandler<FrameEncodedEventArgs> FrameEncoded;

        public Encoder(int width, int height, int frameRate, int bitRate)
        {
            _myFrameEncodedCallback = MyFrameEncodedCallback;
            var result = Environment.Is64BitProcess ? 
                  Interop64.CreateEncoder(out _encoderHandle, width, height, frameRate, bitRate,_myFrameEncodedCallback) : 
                  Interop32.CreateEncoder(out _encoderHandle, width, height, frameRate, bitRate,_myFrameEncodedCallback);
            if (result != 0)
            {
                throw new PusherException(result);
            }
            _layerImage = new Bitmap(width,height);
            _layerGraphics = Graphics.FromImage(_layerImage);
            _layerRect = new Rectangle(0,0,width,height);
            _layerDataSzie = width * height * 4;
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
        /// <param name="image"></param>
        /// <returns></returns>
        public void AddImage(Bitmap image)
        {
            var processImage = _layerImage;
            if (image.Width == _layerImage.Width && image.Height == _layerImage.Height)
            {
                processImage = image;
            }
            else
            {
                _layerGraphics.DrawImage(image, _layerRect);
            }
            var bmpData = processImage.LockBits(_layerRect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            var result = Environment.Is64BitProcess ?
                Interop64.AddImage(_encoderHandle, bmpData.Scan0, _layerDataSzie) :
                Interop32.AddImage(_encoderHandle, bmpData.Scan0, _layerDataSzie);
            if (result != 0)
            {
                throw new PusherException(result);
            }
            processImage.UnlockBits(bmpData);

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
                _layerGraphics.Dispose();
                _layerImage.Dispose();
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
