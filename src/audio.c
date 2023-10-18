#include "audio.h"

#define WAV_IMPLEMENTATION
#include "../include/wav.h"

float gen_sine(float hz, float t)
{
    return sinf(hz * 2.0f * M_PI * t);
}

float gen_square_duty(float hz, float t, float pulseWidth)
{
    if (pulseWidth < 0.0f)
    {
        pulseWidth = 0.0f;
    }
    if (pulseWidth > 1.0f)
    {
        pulseWidth = 1.0f;
    }
    float phase = fmodf(hz * t, 1.0f);
    if (phase < pulseWidth)
    {
        return 1.0f;
    }
    else
    {
        return -1.0f;
    }
}

float gen_square(float hz, float t)
{
    return gen_square_duty(hz, t, 0.5f);
}

float gen_sawtooth(float hz, float t)
{
    float phase = fmodf(hz * t, 1.0f);
    return 2.0f * phase - 1.0f;
}