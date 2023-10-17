
#include "pluxked.h"


// -----------------------------------------------------------------------------

int main(int argc, char **argv)
{
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