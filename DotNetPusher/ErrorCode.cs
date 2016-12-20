namespace DotNetPusher
{
    public enum ErrorCode
    {
        NoCodec = -1,
        OpenCodecFail = -2,
        NoOutFormatContext = -3,
        CreateCodecConextError = -4,
        CreateOutStreamError = -5,
        CopyCodecParamError = -6,
        OpenUrlFail = -7,
        WriteHeaderError = -8,
        WriteFrameError = -9,
        WriteTrailerError = -10,
        CloseUrlFail = -11,
        OpenFormatContextFail = -12,
        NoStream = -13,
        ErrorPackageMode = -14,
        PacketReferenced = -15,
        NoBitStreamFilter = -16,
        CreateBitStreamFilterFail = -17,
        InitBitStreamFilterError = -18,
    }
}
