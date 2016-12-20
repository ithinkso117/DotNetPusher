using System;
using DotNetPusher.VideoPackets;

namespace DotNetPusher.Encoders
{
    public class FrameEncodedEventArgs:EventArgs
    {
        public VideoPacket Packet { get; }

        public FrameEncodedEventArgs(VideoPacket packet)
        {
            Packet = packet;
        }
    }
}
