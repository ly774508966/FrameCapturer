﻿#ifndef FrameCapturer_h
#define FrameCapturer_h

#define fcCLinkage extern "C"
#ifdef _WIN32
    #ifndef fcStaticLink
        #ifdef fcImpl
            #define fcExport __declspec(dllexport)
        #else
            #define fcExport __declspec(dllimport)
        #endif
    #else
        #define fcExport
    #endif
#else
    #define fcExport
#endif

#include <cstdint>

class fcIGraphicsDevice;
class fcIPngContext;
class fcIExrContext;
class fcIGifContext;
class fcIMP4Context;
typedef double fcTime;

enum fcColorSpace
{
    fcColorSpace_RGBA,
    fcColorSpace_I420,
};

enum fcPixelFormat
{
    fcPixelFormat_Unknown,
    fcPixelFormat_RGBA8,
    fcPixelFormat_RGB8,
    fcPixelFormat_RG8,
    fcPixelFormat_R8,
    fcPixelFormat_RGBAHalf,
    fcPixelFormat_RGBHalf,
    fcPixelFormat_RGHalf,
    fcPixelFormat_RHalf,
    fcPixelFormat_RGBAFloat,
    fcPixelFormat_RGBFloat,
    fcPixelFormat_RGFloat,
    fcPixelFormat_RFloat,
    fcPixelFormat_RGBAInt,
    fcPixelFormat_RGBInt,
    fcPixelFormat_RGInt,
    fcPixelFormat_RInt,
};

enum fcTextureFormat
{
    fcTextureFormat_ARGB32 = 0,
    fcTextureFormat_Depth = 1,
    fcTextureFormat_ARGBHalf = 2,
    fcTextureFormat_Shadowmap = 3,
    fcTextureFormat_RGB565 = 4,
    fcTextureFormat_ARGB4444 = 5,
    fcTextureFormat_ARGB1555 = 6,
    fcTextureFormat_Default = 7,
    fcTextureFormat_ARGB2101010 = 8,
    fcTextureFormat_DefaultHDR = 9,
    fcTextureFormat_ARGBFloat = 11,
    fcTextureFormat_RGFloat = 12,
    fcTextureFormat_RGHalf = 13,
    fcTextureFormat_RFloat = 14,
    fcTextureFormat_RHalf = 15,
    fcTextureFormat_R8 = 16,
    fcTextureFormat_ARGBInt = 17,
    fcTextureFormat_RGInt = 18,
    fcTextureFormat_RInt = 19,
};


// -------------------------------------------------------------
// Foundation
// -------------------------------------------------------------

fcCLinkage fcExport void            fcSetModulePath(const char *path);
fcCLinkage fcExport const char*     fcGetModulePath();

fcCLinkage fcExport fcTime          fcGetTime();


// -------------------------------------------------------------
// PNG Exporter
// -------------------------------------------------------------

struct fcPngConfig
{
    int max_active_tasks;
    fcPngConfig() : max_active_tasks(4) {}
};
fcCLinkage fcExport fcIPngContext*  fcPngCreateContext(const fcPngConfig *conf = nullptr);
fcCLinkage fcExport void            fcPngDestroyContext(fcIPngContext *ctx);
fcCLinkage fcExport bool            fcPngExportTexture(fcIPngContext *ctx, const char *path, void *tex, int width, int height, fcTextureFormat fmt, bool flipY = false);
fcCLinkage fcExport bool            fcPngExportPixels(fcIPngContext *ctx, const char *path, const void *pixels, int width, int height, fcPixelFormat fmt, bool flipY = false);


// -------------------------------------------------------------
// EXR Exporter
// -------------------------------------------------------------

struct fcExrConfig
{
    int max_active_tasks;
    fcExrConfig() : max_active_tasks(4) {}
};
fcCLinkage fcExport fcIExrContext*  fcExrCreateContext(const fcExrConfig *conf = nullptr);
fcCLinkage fcExport void            fcExrDestroyContext(fcIExrContext *ctx);
fcCLinkage fcExport bool            fcExrBeginFrame(fcIExrContext *ctx, const char *path, int width, int height);
fcCLinkage fcExport bool            fcExrAddLayerTexture(fcIExrContext *ctx, void *tex, fcTextureFormat fmt, int ch, const char *name, bool flipY = false);
fcCLinkage fcExport bool            fcExrAddLayerPixels(fcIExrContext *ctx, const void *pixels, fcPixelFormat fmt, int ch, const char *name, bool flipY = false);
fcCLinkage fcExport bool            fcExrEndFrame(fcIExrContext *ctx);


// -------------------------------------------------------------
// GIF Exporter
// -------------------------------------------------------------

struct fcGifConfig
{
    int width;
    int height;
    int num_colors;
    int max_active_tasks;
    fcGifConfig()
        : width(), height(), num_colors(256), max_active_tasks(8) {}
};
fcCLinkage fcExport fcIGifContext*  fcGifCreateContext(const fcGifConfig *conf);
fcCLinkage fcExport void            fcGifDestroyContext(fcIGifContext *ctx);
fcCLinkage fcExport bool            fcGifAddFrame(fcIGifContext *ctx, void *tex);
// timestamp=-1 is treated as current time.
fcCLinkage fcExport bool            fcGifAddFrameTexture(fcIGifContext *ctx, void *tex, fcTextureFormat fmt, bool keyframe = false, fcTime timestamp = -1.0);
// timestamp=-1 is treated as current time.
fcCLinkage fcExport bool            fcGifAddFramePixels(fcIGifContext *ctx, const void *pixels, fcPixelFormat fmt, bool keyframe = false, fcTime timestamp = -1.0);
fcCLinkage fcExport void            fcGifClearFrame(fcIGifContext *ctx);
fcCLinkage fcExport bool            fcGifWriteFile(fcIGifContext *ctx, const char *path, int begin_frame = 0, int end_frame = -1);
fcCLinkage fcExport int             fcGifWriteMemory(fcIGifContext *ctx, void *buf, int begin_frame = 0, int end_frame = -1);
fcCLinkage fcExport int             fcGifGetFrameCount(fcIGifContext *ctx);
fcCLinkage fcExport void            fcGifGetFrameData(fcIGifContext *ctx, void *tex, int frame);
fcCLinkage fcExport int             fcGifGetExpectedDataSize(fcIGifContext *ctx, int begin_frame, int end_frame);
fcCLinkage fcExport void            fcGifEraseFrame(fcIGifContext *ctx, int begin_frame, int end_frame);


// -------------------------------------------------------------
// MP4 Exporter
// -------------------------------------------------------------

#ifndef fcImpl
struct fcStream;
#endif
// function types for custom stream
typedef size_t (*fcTellp_t)(void *obj);
typedef void   (*fcSeekp_t)(void *obj, size_t pos);
typedef size_t (*fcWrite_t)(void *obj, const void *data, size_t len);

struct fcBufferData
{
    void *data;
    size_t size;

    fcBufferData() : data(), size() {}
};

fcCLinkage fcExport fcStream*       fcCreateFileStream(const char *path);
fcCLinkage fcExport fcStream*       fcCreateMemoryStream();
fcCLinkage fcExport fcStream*       fcCreateCustomStream(void *obj, fcTellp_t tellp, fcSeekp_t seekp, fcWrite_t write);
fcCLinkage fcExport void            fcDestroyStream(fcStream *s);
fcCLinkage fcExport fcBufferData    fcStreamGetBufferData(fcStream *s); // s must be created by fcCreateMemoryStream(), otherwise return {nullptr, 0}.
fcCLinkage fcExport uint64_t        fcStreamGetWrittenSize(fcStream *s);

struct fcMP4Config
{
    bool    video;
    bool    audio;
    bool    video_use_hardware_encoder_if_possible;
    int     video_width;
    int     video_height;
    int     video_bitrate;
    int     video_max_framerate;
    int     video_max_buffers;
    float   audio_scale; // useful for scaling (-1.0 - 1.0) samples to (-32767.0f - 32767.0f)
    int     audio_sample_rate;
    int     audio_num_channels;
    int     audio_bitrate;

    fcMP4Config()
        : video(true), audio(true)
        , video_use_hardware_encoder_if_possible(true)
        , video_width(), video_height()
        , video_bitrate(256000), video_max_framerate(60), video_max_buffers(8)
        , audio_scale(1.0f), audio_sample_rate(48000), audio_num_channels(2), audio_bitrate(64000)
    {}
};

enum fcDownloadState {
    fcDownloadState_Idle,
    fcDownloadState_Completed,
    fcDownloadState_Error,
    fcDownloadState_InProgress,
};
fcCLinkage fcExport bool            fcMP4DownloadCodecBegin();
fcCLinkage fcExport fcDownloadState fcMP4DownloadCodecGetState();

fcCLinkage fcExport fcIMP4Context*  fcMP4CreateContext(fcMP4Config *conf);
fcCLinkage fcExport void            fcMP4DestroyContext(fcIMP4Context *ctx);
fcCLinkage fcExport void            fcMP4AddOutputStream(fcIMP4Context *ctx, fcStream *stream);
// timestamp=-1 is treated as current time.
fcCLinkage fcExport bool            fcMP4AddVideoFrameTexture(fcIMP4Context *ctx, void *tex, fcTime timestamp = -1);
// timestamp=-1 is treated as current time.
fcCLinkage fcExport bool            fcMP4AddVideoFramePixels(fcIMP4Context *ctx, const void *pixels, fcColorSpace cs, fcTime timestamp = -1.0);
// timestamp=-1 is treated as current time.
fcCLinkage fcExport bool            fcMP4AddAudioFrame(fcIMP4Context *ctx, const float *samples, int num_samples, fcTime timestamp = -1.0);

#endif // FrameCapturer_h
