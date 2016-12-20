using System;
using System.Runtime.InteropServices;

namespace DotNetPusher.VideoFrames
{
    public sealed partial class VideoFrame
    {
        private static class Interop64
        {

            [DllImport("x64\\NativePusher.x64.dll", EntryPoint = "GetFrameIndex", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern int GetFrameIndex(IntPtr hFrame);

            [DllImport("x64\\NativePusher.x64.dll", EntryPoint = "GetFrameSize", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern int GetFrameSize(IntPtr hFrame);

            [DllImport("x64\\NativePusher.x64.dll", EntryPoint = "GetFrameData", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern IntPtr GetFrameData(IntPtr hFrame);

            [DllImport("x64\\NativePusher.x64.dll", EntryPoint = "DestroyFrame", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern void DestroyFrame(IntPtr hFrame);
        }
    }
}
