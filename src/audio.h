#ifndef _AUDIO_H
#define _AUDIO_H

#include "../include/dr_wav.h"

// -----------------------------------------------------------------------------

#include "plx.h"

// -----------------------------------------------------------------------------

typedef enum plx_format
{
    INVALID,
    WAV,
    MP3,
    FLAC,
    PLX_FORMAT_COUNT
} plx_format;

typedef struct plx_audio
{
    const char *filepath;
    plx_format fileformat;
    u64 container;
    u32 format;
    u32 channels;
    u32 sampleRate;
    u32 bitsPerSample;
    u64 totalFrameCount;
    u64 channelFrameCount;
    f32 **frames;
} plx_audio;

// -----------------------------------------------------------------------------

i32 *interleave(const f32 **inFrames, u64 channels, u64 channelFrameCount);

f32 **deinterleave(const i32 *inFrames, u64 channels, u64 channelFrameCount);

// -----------------------------------------------------------------------------

plx_audio *plx_audio_loadf(const char *filepath);
bool plx_audio_dumpf(const plx_audio *audio, const char *filepath);
void plx_audio_free(plx_audio *audio);

// -----------------------------------------------------------------------------

#endif