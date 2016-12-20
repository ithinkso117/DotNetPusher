using System;
using System.Runtime.InteropServices;

namespace DotNetPusher.Pushers
{
    public partial class Pusher
    {
        private static class Interop32
        {
            [DllImport("x86\\NativePusher.x86.dll", EntryPoint = "CreatePusher", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern int CreatePusher(out IntPtr hPushr);

            [DllImport("x86\\NativePusher.x86.dll", EntryPoint = "DestroyPusher", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern void DestroyPusher(IntPtr hPushr);

            [DllImport("x86\\NativePusher.x86.dll", EntryPoint = "StartPush", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern int StartPush(IntPtr hPushr, string url, int width, int height, int frameRate);

            [DllImport("x86\\NativePusher.x86.dll", EntryPoint = "StopPush", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern int StopPush(IntPtr hPushr);

            [DllImport("x86\\NativePusher.x86.dll", EntryPoint = "PushPacket", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
            public static extern int PushPacket(IntPtr hPushr, IntPtr hPacket);
        }
    }
}
