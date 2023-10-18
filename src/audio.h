#ifndef _AUDIO_H
#define _AUDIO_H

#include "plx.h"

#include "../include/wav.h"

float gen_sine(float hz, float t);
float gen_square_duty(float hz, float t, float pulseWidth);
float gen_square(float hz, float t);
float gen_sawtooth(float hz, float t);

#endif