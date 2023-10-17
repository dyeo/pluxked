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
    drwav_data_format *wavformat = malloc(sizeof(drwav_data_format));
    *wavformat                   = (drwav_data_format){wav.container,
                                                       wav.translatedFormatTag,
                                                       wav.channels,
                                                       wav.sampleRate,
                                                       wav.bitsPerSample};
    plx_audio *audio             = malloc(sizeof(plx_audio));
    *audio                       = (plx_audio){filepath,
                                               WAV,
                                               wavformat,
                                               wav.totalPCMFrameCount,
                                               malloc(wav.totalPCMFrameCount * sizeof(i32))};
    drwav_read_pcm_frames(&wav, audio->frameCount, audio->frames);
    return audio;
}

bool plx_audio_dumpf(const plx_audio *audio, const char *filepath)
{
    drwav wav;
    if (!drwav_init_file_write(&wav, filepath, audio->wav, NULL))
    {
        return false;
    }
    drwav_write_pcm_frames(&wav, audio->frameCount, audio->frames);
    return true;
}

void plx_audio_free(plx_audio *audio)
{
    if (audio == NULL || audio->frames == NULL)
    {
        return;
    }
    switch (audio->format)
    {
        case WAV:
            free(audio->wav);
            break;

        default:
            break;
    }
    free(audio->frames);
    *audio = (plx_audio){NULL, INVALID, NULL, 0, NULL};
}

// -----------------------------------------------------------------------------

#endif