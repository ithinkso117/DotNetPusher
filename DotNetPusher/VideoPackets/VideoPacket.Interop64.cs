using System;
using System.Runtime.InteropServices;

namespace DotNetPusher.VideoPackets
{
    public partial class VideoPacket
    {
        private static class Interop64
        {
            [DllImport("x64\\NativePusher.x64.dll", EntryPoint = "GetPacketIndex", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern int GetPacketIndex(IntPtr hPacket);

            [DllImport("x64\\NativePusher.x64.dll", EntryPoint = "GetPacketSize", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern int GetPacketSize(IntPtr hPacket);

            [DllImport("x64\\NativePusher.x64.dll", EntryPoint = "GetPacketData", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern IntPtr GetPacketData(IntPtr hPacket);

            [DllImport("x64\\NativePusher.x64.dll", EntryPoint = "DestroyPacket", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern int DestroyPacket(IntPtr hPacket);
        }
    }
}
