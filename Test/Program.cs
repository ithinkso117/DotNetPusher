using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Threading;
using DotNetPusher.Encoders;
using DotNetPusher.Pushers;

namespace Test
{
    class Program
    {
        [DllImport("user32.dll")]
        public static extern int GetSystemMetrics(int index);

        [DllImport("user32.dll")]
        private static extern IntPtr GetDC(IntPtr hWnd);

        [DllImport("gdi32.dll")]
        private static extern bool BitBlt([In] IntPtr hdc, int nXDest, int nYDest, int nWidth, int nHeight, [In] IntPtr hdcSrc, int nXSrc, int nYSrc, uint dwRop);

        [DllImport("user32.dll")]
        private static extern bool ReleaseDC(IntPtr hWnd, IntPtr hdc);


        private static void Main()
        {
            //The url to push.
            var pushUrl = "rtmp://localhost:1935/live/stream";

            //Push frame rate.
            var frameRate = 15;
            var waitInterval = 1000 / frameRate;


            var screenWidth = GetSystemMetrics(0);
            var screenHeight = GetSystemMetrics(1);

            //Define the width and height
            var width = screenWidth;
            var height = screenHeight;

            var pusher = new Pusher();
            pusher.StartPush(pushUrl, width, height, frameRate);
            
            var stopEvent = new ManualResetEvent(false);

            var thread = new Thread(() =>
            {
                var encoder = new Encoder(width, height, frameRate, 1024*800);
                encoder.FrameEncoded += (sender, e) =>
                {
                    //A frame encoded.
                    var packet = e.Packet;
                    pusher.PushPacket(packet);
                    Console.WriteLine($"Packet pushed, size:{packet.Size}.");
                };
                var screenDc = GetDC(IntPtr.Zero);
                var bitmap = new Bitmap(screenWidth, screenHeight);

                try
                {
                    while (!stopEvent.WaitOne(1))
                    {
                        var start = Environment.TickCount;
                        using (var graphic = Graphics.FromImage(bitmap))
                        {
                            var imageDc = graphic.GetHdc();
                            BitBlt(imageDc, 0, 0, width, height, screenDc, 0, 0, 0x00CC0020);
                        }
                        encoder.AddImage(bitmap);
                        var timeUsed = Environment.TickCount - start;
                        var timeToWait = waitInterval - timeUsed;
                        Thread.Sleep(timeToWait<0? 0: timeToWait);
                    }
                    encoder.Flush();
                }
                finally
                {
                    encoder.Dispose();
                    bitmap.Dispose();
                    ReleaseDC(IntPtr.Zero, screenDc);
                }
            });
           
            thread.Start();

            Console.ReadLine();

            stopEvent.Set();

            thread.Join();
            pusher.StopPush();
            pusher.Dispose();

            Console.WriteLine("Stopped!");
            Console.ReadLine();

        }
    }
}
