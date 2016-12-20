using System;

namespace DotNetPusher
{
    public class PusherException:Exception
    {
        public ErrorCode Code { get; }

        public PusherException(int errorCode)
            : base(GetErrorMessage(errorCode))
        {
            Code = (ErrorCode)errorCode;
        }

        private static string GetErrorMessage(int errorCode)
        {
            var code = (ErrorCode)errorCode;
            switch (code)
            {
                case ErrorCode.NoCodec:
                    return "Can not find the codec.";
                case ErrorCode.OpenCodecFail:
                    return "Open codec fail.";
                case ErrorCode.NoOutFormatContext:
                    return "Can not find output format conext.";
                case ErrorCode.CreateCodecConextError:
                    return "Create codec conext error.";
                case ErrorCode.CreateOutStreamError:
                    return "Can not create the out stream";
                case ErrorCode.CopyCodecParamError:
                    return "Copy codec params error";
                case ErrorCode.OpenUrlFail:
                    return "Open url fail.";
                case ErrorCode.WriteHeaderError:
                    return "Write header error.";
                case ErrorCode.WriteFrameError:
                    return "Write frame error.";
                case ErrorCode.WriteTrailerError:
                    return "Write trailer error.";
                case ErrorCode.CloseUrlFail:
                    return "Close url fail.";
                case ErrorCode.OpenFormatContextFail:
                    return "Open format context fail.";
                case ErrorCode.NoStream:
                    return "Can not find stream in package";
                case ErrorCode.ErrorPackageMode:
                    return "Package mode error.";
                case ErrorCode.PacketReferenced:
                    return "Packet is referenced, can not be disposed.";
                case ErrorCode.NoBitStreamFilter:
                    return "Can not find bit stream filter.";
                case ErrorCode.CreateBitStreamFilterFail:
                    return "Create bit stream filter fail.";
                case ErrorCode.InitBitStreamFilterError:
                    return "Init bit stream filter fail.";
                default:
                    return string.Empty;
            }
        }
    }
}
