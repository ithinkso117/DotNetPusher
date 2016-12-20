using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using DotNetPusher.Pushers;
using Encoder = DotNetPusher.Encoders.Encoder;

namespace Test
{
    class Program
    {
        private static void Main(string[] args)
        {
            //The url to push.
            var pushUrl = "rtmp://localhost:1395/live/stream";

            //Push frame rate.
            var frameRate = 24;

            //Define the width and height
            var width = 1280;
            var height = 720;

            //All images in the folder should have the same width and height.
            var imageFolder = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Images");
            var images = Directory.GetFiles(imageFolder);

            Console.WriteLine("Press any key to start push.");
            Console.ReadLine();

            var pusher = new Pusher();
            pusher.StartPush(pushUrl, width, height, frameRate);
            var encoder = new Encoder(width, height, frameRate);
            encoder.FrameEncoded += (sender, e) =>
            {
                //Onec a frame encoded.
                var packet = e.Packet;
                pusher.PushPacket(packet);
                Console.WriteLine($"Packet pushed, size:{packet.Size}.");
            };

            for (int x = 0; x < 1; x++)
            {
                for (int i = 0; i < images.Length; i++)
                {
                    var bitmap = new Bitmap(images[i]);
                    Graphics g = Graphics.FromImage(bitmap);
                    g.DrawString(((x* images.Length) + i).ToString(), new Font(FontFamily.GenericSansSerif, 20, FontStyle.Bold), Brushes.LawnGreen, new PointF(1, 20));
                    g.Dispose();
                    Rectangle rect = new Rectangle(0, 0, width, height);
                    BitmapData bmpData = bitmap.LockBits(rect, ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);
                    IntPtr ptr = bmpData.Scan0;
                    int pixels = width*height;
                    byte[] rgbValues = new byte[pixels*4];
                    Marshal.Copy(ptr, rgbValues, 0, pixels*4);
                    bitmap.UnlockBits(bmpData);
                    encoder.AddImage(rgbValues);
                    bitmap.Dispose();
                }
            }
            //Flush
            encoder.Flush();

            pusher.StopPush();
            pusher.Dispose();
            encoder.Dispose();
            Console.WriteLine("Finished!");
            Console.ReadLine();
        }
    }
}
