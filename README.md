# DotNetPusher
A .NET library for pushing rtmp video stream.


This is a library writen in C# and C++ which uses FFMpeg to push video stream to a existing rtmp server.
You can use it on windows, It supports x86 and x64 windows system.

这是一个用C#和C++实现的调用FFMpeg向某个rtmp服务器推送视频流的库。
你可以在Windows系统上使用，它支持32位和64位的Windows。

I notice that there is a "Wall" between the "newbie" and the "master" who are using FFMpeg.
It's easy to use FFMpeg.exe to push video stream to a rtmp server, but no answer for using code with FFMpeg's libraries. 
People who knows the usage never share their knowledge.
Most of the code you found in internet do not work :(.

我注意到有一堵无形的墙挡在使用FFmpeg的“新人”和“大师”

So I decide share this project for people who needs this. 

所以我决定将这个东西分享给需要的人。

Usage:
The usage is very simple, just need to set the push url, and add image into it.

用起来很简单，设置下推送的url，然后往里面填充image吧。

var pusher = new Pusher();
pusher.StartPush(pushUrl, width, height, frameRate);
var encoder = new Encoder(width, height, frameRate);
encoder.FrameEncoded += (sender, e) =>
{
	//A frame encoded.
	var packet = e.Packet;
	pusher.PushPacket(packet);
	Console.WriteLine($"Packet pushed, size:{packet.Size}.");
};
//Push your images.
for (int i = 0; i < images.Length; i++)
{
	var bitmap = new Bitmap(images[i]);
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
 //Flush
encoder.Flush();
//Dispose
pusher.StopPush();
pusher.Dispose();
encoder.Dispose();

Any question please contact me with email: ithinkso117#163.com.
任何问题欢迎联系我共同探讨，邮箱：ithinkso117#163.com
