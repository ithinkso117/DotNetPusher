using System;
using System.Runtime.InteropServices;

namespace DotNetPusher.VideoFrames
{
    public sealed partial class VideoFrame
    {
        private static class Interop32
        {

            [DllImport("x86\\NativePusher.x86.dll", EntryPoint = "GetFrameIndex", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern int GetFrameIndex(IntPtr hFrame);

            [DllImport("x86\\NativePusher.x86.dll", EntryPoint = "GetFrameSize", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern int GetFrameSize(IntPtr hFrame);

            [DllImport("x86\\NativePusher.x86.dll", EntryPoint = "GetFrameData", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern IntPtr GetFrameData(IntPtr hFrame);

            [DllImport("x86\\NativePusher.x86.dll", EntryPoint = "DestroyFrame", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern void DestroyFrame(IntPtr hFrame);

        }
    }
}
