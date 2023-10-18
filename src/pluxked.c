#define WAV_IMPLEMENTATION
#include "pluxked.h"

float gen_sine(float hz, float t)
{
    return sinf(hz * 2.0f * M_PI * t);
}

float gen_square_pwm(float hz, float t, float pulseWidth)
{
    if (pulseWidth < 0.0f)
        pulseWidth = 0.0f;
    if (pulseWidth > 1.0f)
        pulseWidth = 1.0f;

    float phase = fmodf(hz * t, 1.0f);
    if (phase < pulseWidth)
        return 1.0f;
    else
        return -1.0f;
}

float gen_square(float hz, float t)
{
    return gen_square_pwm(hz, t, 0.5f);
}

float gen_sawtooth(float hz, float t)
{
    float phase = fmodf(hz * t, 1.0f);
    return 2.0f * phase - 1.0f;
}

int16_t gen_440hz_sine(uint32_t channels,
                       uint32_t sampleRate,
                       uint32_t sampleCount,
                       int32_t channelIndex,
                       int32_t sampleIndex)
{
    (void) channels;
    (void) sampleCount;
    (void) channelIndex;
    float t = (float) sampleIndex / (float) sampleRate;
    return wav_float32_to_pcm16(gen_sine(440.0f, t)* 0.1f);
}

int16_t gen_440hz_square(uint32_t channels,
                         uint32_t sampleRate,
                         uint32_t sampleCount,
                         int32_t channelIndex,
                         int32_t sampleIndex)
{
    (void) channels;
    (void) sampleCount;
    (void) channelIndex;
    float t = (float) sampleIndex / (float) sampleRate;
    return wav_float32_to_pcm16(gen_square_pwm(440.0f, t, 0.25f) * 0.1f);
}

int16_t gen_440hz_sawtooth(uint32_t channels,
                           uint32_t sampleRate,
                           uint32_t sampleCount,
                           int32_t channelIndex,
                           int32_t sampleIndex)
{
    (void) channels;
    (void) sampleCount;
    (void) channelIndex;
    float t = (float) sampleIndex / (float) sampleRate;
    return wav_float32_to_pcm16(gen_sawtooth(440.0f, t)* 0.1f);
}

// -----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    wav_audio *sine = wav_gen_pcm16(2, wav_44100hz, 44100 * 5, gen_440hz_sine);
    wav_dumpf("sine.wav", sine);

    wav_audio *sawtooth =
        wav_gen_pcm16(2, wav_44100hz, 44100 * 5, gen_440hz_sawtooth);
    wav_dumpf("sawtooth.wav", sawtooth);

    wav_audio *square =
        wav_gen_pcm16(2, wav_44100hz, 44100 * 5, gen_440hz_square);
    wav_dumpf("square.wav", square);

    for (int i = 1; i < argc; ++i)
    {
        char buf[128];
        snprintf(buf, 128, "out_%d.wav", i);
        printf("Copying WAV data from %s to %s...\n", argv[i], buf);
        wav_audio *a = wav_loadf(argv[i]);
        if (a != NULL)
        {
            printf("%dx%d-type, %u samples.\n",
                   a->channels,
                   a->format,
                   a->sampleCount);
            wav_dumpf(buf, a);
        }
        else
        {
            fprintf(stderr,
                    "FATAL: Unspecified load failure for file: %s\n",
                    argv[i]);
            exit(1);
        }
    }
}