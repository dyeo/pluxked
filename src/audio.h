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
    i32 **frames;
} plx_audio;

// -----------------------------------------------------------------------------

static inline i32 *
interleave(const int32_t **inFrames, size_t channels, size_t channelFrameCount)
{
    i32 *outFrames = (i32 *) malloc(sizeof(i32) * channels * channelFrameCount);
    if (outFrames == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < channelFrameCount; ++i)
    {
        for (size_t ch = 0; ch < channels; ++ch)
        {
            outFrames[i * channels + ch] = inFrames[ch][i];
        }
    }
    return outFrames;
}

static inline i32 **
deinterleave(const int32_t *inFrames, size_t channels, size_t channelFrameCount)
{
    i32 **outFrames = (i32 **) malloc(sizeof(i32 *) * channels);
    if (outFrames == NULL)
    {
        return NULL;
    }

    for (size_t ch = 0; ch < channels; ++ch)
    {
        outFrames[ch] = (i32 *) malloc(sizeof(i32) * channelFrameCount);
        if (outFrames[ch] == NULL)
        {
            for (size_t j = 0; j < ch; ++j)
            {
                free(outFrames[j]);
            }
            free(outFrames);
            return NULL;
        }

        for (size_t i = 0; i < channelFrameCount; ++i)
        {
            outFrames[ch][i] = inFrames[i * channels + ch];
        }
    }
    return outFrames;
}
// -----------------------------------------------------------------------------

plx_audio *plx_audio_loadf(const char *filepath);
bool plx_audio_dumpf(const plx_audio *audio, const char *filepath);
void plx_audio_free(plx_audio *audio);

// -----------------------------------------------------------------------------

#endif