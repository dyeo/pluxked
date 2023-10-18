#ifndef _AUDIO_H
#define _AUDIO_H

#include "plx.h"

#include "../include/wav.h"

float plx_gen_sine(float hz, float t);
float plx_gen_square_duty(float hz, float t, float pulseWidth);
float plx_gen_square(float hz, float t);
float plx_gen_saw(float hz, float t);

#endif