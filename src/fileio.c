#include "fileio.h"

#if PLX_WINDOWS
#define WIN32_MEAN_AND_LEAN 1
#define NOGDI 1
#include <Shlwapi.h>
#include <Windows.h>
#include <io.h>
#elif PLX_POSIX
#include <unistd.h>
#endif

bool fexists(const char *filename)
{
#ifdef PLX_WINDOWS
    return _access(filename, 0) == 0;
#elif PLX_POSIX
    return access(filename, F_OK) == 0;
#else
    return false;
#endif
}

char *fmimetype(const char *filepath)
{
#if PLX_WINDOWS
    {
        char mimebuf[256];
        char extbuf[128];
        DWORD bufferSize = sizeof(mimebuf);

        // Extract the file extension
        char *dot = strrchr(filepath, '.');
        if (dot)
        {
            strcpy(extbuf, dot);
        }
        else
        {
            fprintf(stderr, "Cannot determine file extension.\n");
            return NULL;
        }

        if (AssocQueryString(
                0, ASSOCSTR_CONTENTTYPE, extbuf, NULL, mimebuf, &bufferSize) !=
            S_OK)
        {
            fprintf(stderr, "Failed to retrieve MIME type.\n");
            return NULL;
        }

        size_t slen = strnlen(mimebuf, bufferSize - 1);
        char *mime  = malloc(slen + 1);
        strncpy(mime, mimebuf, slen);
        mime[slen + 1] = '\0';
        return mime;
    }
#elif PLX_POSIX
    {
        char command[1024];
        snprintf(command, sizeof(command), "file --brief --mime %s", filepath);
        FILE *fp = popen(command, "r");
        if (fp == NULL)
        {
            perror("popen");
            return NULL;
        }
        char buf[256];
        if (fgets(buf, sizeof(buf), fp) == NULL)
        {
            perror("fgets");
            return NULL;
        }
        pclose(fp);
        size_t slen = strnlen(&buf, 255);
        char *mime  = malloc(slen + 1);
        strncpy(mime, buf, slen);
        mime[slen + 1] = '\0';
        return mime;
    }
#else
    {
        (void) filepath;
        return NULL;
    }
#endif
}