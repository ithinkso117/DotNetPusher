using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using DotNetPusher.Pushers;
using Encoder = DotNetPusher.Encoders.Encoder;

namespace Test
{
    class Program
    {
        private static void Main(string[] args)
        {
            //The url to push.
            var pushUrl = "rtmp://localhost:1935/live/stream";

            //Push frame rate.
            var frameRate = 24;

            var screenSize = new Size(Screen.PrimaryScreen.Bounds.Width, Screen.PrimaryScreen.Bounds.Height);

            //Define the width and height
            var width = screenSize.Width;
            var height = screenSize.Height;

            Console.WriteLine("Press any key to start push.");
            Console.ReadLine();

            var pusher = new Pusher();
            pusher.StartPush(pushUrl, width, height, frameRate);
            var encoder = new Encoder(width, height, frameRate, 8000);
            encoder.FrameEncoded += (sender, e) =>
            {
                //A frame encoded.
                var packet = e.Packet;
                pusher.PushPacket(packet);
                Console.WriteLine($"Packet pushed, size:{packet.Size}.");
            };

            var bitmap = new Bitmap(screenSize.Width, screenSize.Height);
            var graphic = Graphics.FromImage(bitmap);
            bool stop = false;
            Task.Run(() =>
            {
                while (!stop)
                {
                    graphic.CopyFromScreen(0, 0, 0, 0, screenSize);
                    Rectangle rect = new Rectangle(0, 0, width, height);
                    BitmapData bmpData = bitmap.LockBits(rect, ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);
                    IntPtr ptr = bmpData.Scan0;
                    int pixels = width * height;
                    byte[] rgbValues = new byte[pixels * 4];
                    Marshal.Copy(ptr, rgbValues, 0, pixels * 4);
                    bitmap.UnlockBits(bmpData);
                    encoder.AddImage(rgbValues);
                    Thread.Sleep(1000/24);
                }
            });
            Console.ReadLine();
            stop = true;

            Thread.Sleep(1000);

            //Flush
            encoder.Flush();
            pusher.StopPush();
            pusher.Dispose();
            encoder.Dispose();

            graphic.Dispose();
            bitmap.Dispose();

            Console.WriteLine("Finished!");
            Console.ReadLine();

        }
    }
}
