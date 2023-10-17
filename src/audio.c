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
    audio->frames = deinterleave(raw, audio->channels, audio->channelFrameCount);
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