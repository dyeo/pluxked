
#include "pluxked.h"

// -----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        char buf[128];
        snprintf(buf, 128, "out_%d.wav", i);
        printf("%s -> %s... ", argv[i], buf);
        plx_audio *a = plx_audio_loadf(argv[i]);
        if (a != NULL)
        {
            plx_audio_dumpf(a, buf);
            plx_audio_free(a);
            printf("Done\n");
        }
        else
        {
            fprintf(stderr,
                    "ERROR: Unspecified load failure for file: %s\n",
                    argv[i]);
        }
    }
}