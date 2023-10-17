#ifndef _AUDIO_C
#define _AUDIO_C

// -----------------------------------------------------------------------------

#define DR_WAV_IMPLEMENTATION
#include "audio.h"
#include "fileio.h"

#include "plx.h"

// -----------------------------------------------------------------------------
// audio manipulation
// -----------------------------------------------------------------------------

i32 *interleave(const f32 **inFrames, u64 channels, u64 channelFrameCount)
{
    i32 *outFrames = (i32 *) malloc(sizeof(i32) * channels * channelFrameCount);
    if (outFrames == NULL)
    {
        return NULL;
    }

    for (u64 i = 0; i < channelFrameCount; ++i)
    {
        for (u64 ch = 0; ch < channels; ++ch)
        {
            outFrames[i * channels + ch] = (i32) (inFrames[ch][i] * 32767.0f);
        }
    }
    return outFrames;
}

f32 **deinterleave(const i32 *inFrames, u64 channels, u64 channelFrameCount)
{
    f32 **outFrames = (f32 **) malloc(sizeof(f32 *) * channels);
    if (outFrames == NULL)
    {
        return NULL;
    }

    for (u64 ch = 0; ch < channels; ++ch)
    {
        outFrames[ch] = (f32 *) malloc(sizeof(f32) * channelFrameCount);
        if (outFrames[ch] == NULL)
        {
            for (u64 j = 0; j < ch; ++j)
            {
                free(outFrames[j]);
            }
            free(outFrames);
            return NULL;
        }

        for (u64 i = 0; i < channelFrameCount; ++i)
        {
            outFrames[ch][i] = (f32) inFrames[i * channels + ch] / 32767.0f;
        }
    }
    return outFrames;
}

plx_audio *plx_audio_loadf(const char *filepath)
{
    drwav wav;
    if (!drwav_init_file(&wav, filepath, NULL))
    {
        return NULL;
    }
    plx_audio *audio = malloc(sizeof(plx_audio));
    *audio           = (plx_audio){filepath,
                                   WAV,
                                   wav.container,
                                   wav.translatedFormatTag,
                                   wav.channels,
                                   wav.sampleRate,
                                   wav.bitsPerSample,
                                   wav.totalPCMFrameCount,
                                   wav.totalPCMFrameCount / wav.channels,
                                   NULL};
    i32 *raw         = malloc(audio->totalFrameCount * sizeof(i32));
    drwav_read_pcm_frames(&wav, audio->totalFrameCount, raw);
    audio->frames =
        deinterleave(raw, audio->channels, audio->channelFrameCount);
    free(raw);
    return audio;
}

bool plx_audio_dumpf(const plx_audio *audio, const char *filepath)
{
    drwav wav;
    drwav_data_format format = {audio->container,
                                audio->format,
                                audio->channels,
                                audio->sampleRate,
                                audio->bitsPerSample};
    if (!drwav_init_file_write(&wav, filepath, &format, NULL))
    {
        return false;
    }
    i32 *raw = interleave((const f32 **) audio->frames,
                          audio->channels,
                          audio->channelFrameCount);
    drwav_write_pcm_frames(&wav, audio->totalFrameCount, raw);
    free(raw);
    return true;
}

void plx_audio_free(plx_audio *audio)
{
    if (audio == NULL || audio->frames == NULL)
    {
        return;
    }
    for (u64 c = 0; c < audio->channels; ++c)
    {
        free(audio->frames[c]);
    }
    free(audio->frames);
    *audio = (plx_audio){NULL, INVALID, 0, 0, 0, 0, 0, 0, 0, NULL};
}

// -----------------------------------------------------------------------------

#endif