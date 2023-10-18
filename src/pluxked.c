#include "pluxked.h"

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
    return wav_float32_to_pcm16(plx_gen_sine(440.0f, t) * 0.1f);
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
    return wav_float32_to_pcm16(plx_gen_square_duty(440.0f, t, 0.25f) * 0.1f);
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
    return wav_float32_to_pcm16(plx_gen_saw(440.0f, t) * 0.1f);
}

// -----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    wav_audio *sine = wav_gen_pcm16(2, wav_44100hz, 44100 * 5, gen_440hz_sine);
    wav_dumpf("sine.wav", sine);

    wav_play(sine);

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