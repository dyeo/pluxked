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
    plx_format format;
    drwav_data_format *wav;
    u64 frameCount;
    i32 *frames;
} plx_audio;

// -----------------------------------------------------------------------------

plx_audio *plx_audio_loadf(const char *filepath);
bool plx_audio_dumpf(const plx_audio *audio, const char *filepath);
void plx_audio_free(plx_audio *audio);

// -----------------------------------------------------------------------------

#endif