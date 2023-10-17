#ifndef _WAV_H
#define _WAV_H

#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------

typedef enum wav_sampleformat
{
    wav_int16,
    wav_int32,
    wav_float32,
} wav_sampleformat;

typedef struct wav_audio
{
    wav_sampleformat format;
    uint16_t channels;
    uint32_t sampleRate;
    uint32_t sampleCount;
    union
    {
        int16_t **int16;
        int32_t **int32;
        float **float32;
    };
} wav_audio;

// -----------------------------------------------------------------------------

extern wav_audio *wav_loadb(size_t len, uint8_t *data);
extern uint8_t *wav_dumpb(const wav_audio *wav, size_t *len);
extern uint8_t *wav_dumpb(const wav_audio *wav, size_t *len);
extern bool wav_dumpf(const char *filename, const wav_audio *wav);
extern void wav_free(wav_audio *wav);

// -----------------------------------------------------------------------------

#define wav_invalid 0

typedef enum wav_audioformat
{
    wav_pcm      = 0x0001,
    wav_adpcm    = 0x0002,
    wav_float    = 0x0003,
    wav_extended = 0xFFFE
} wav_audioformat;

typedef enum wav_samplerate
{
    wav_8000hz   = 8000,
    wav_11025hz  = 11025,
    wav_16000hz  = 16000,
    wav_22050hz  = 22050,
    wav_32000hz  = 32000,
    wav_44100hz  = 44100,
    wav_48000hz  = 48000,
    wav_88200hz  = 88200,
    wav_96000hz  = 96000,
    wav_176400hz = 176400,
    wav_192000hz = 192000,
    wav_384000hz = 384000
} wav_samplerate;

typedef enum plx_wav_bitspersample
{
    plx_wav_16bit = 16,
    plx_wav_24bit = 24,
    plx_wav_32bit = 32,
} plx_wav_bitspersample;

// -----------------------------------------------------------------------------

#define WAV_INTERLEAVE(                                                        \
    I_FRAMES, I_TYPE, O_FRAMES, O_TYPE, CHANNELS, FRAMECOUNT)                  \
    do                                                                         \
    {                                                                          \
        O_FRAMES = (O_TYPE *) malloc(sizeof(O_TYPE) * CHANNELS * FRAMECOUNT);  \
        if (O_FRAMES == NULL)                                                  \
            break;                                                             \
        for (size_t i = 0; i < FRAMECOUNT; ++i)                                \
        {                                                                      \
            for (size_t ch = 0; ch < CHANNELS; ++ch)                           \
            {                                                                  \
                O_FRAMES[(i * CHANNELS) + ch] = I_FRAMES[ch][i];               \
            }                                                                  \
        }                                                                      \
    } while (0)

#define WAV_DEINTERLEAVE(                                                      \
    I_FRAMES, I_TYPE, O_FRAMES, O_TYPE, CHANNELS, FRAMECOUNT)                  \
    do                                                                         \
    {                                                                          \
        O_FRAMES = (O_TYPE **) malloc(sizeof(O_TYPE *) * CHANNELS);            \
        if (O_FRAMES == NULL)                                                  \
            break;                                                             \
        for (size_t ch = 0; ch < CHANNELS; ++ch)                               \
        {                                                                      \
            O_FRAMES[ch] = (O_TYPE *) malloc(sizeof(O_TYPE) * FRAMECOUNT);     \
            if (O_FRAMES[ch] == NULL)                                          \
            {                                                                  \
                for (size_t j = 0; j < ch; ++j)                                \
                {                                                              \
                    free(O_FRAMES[j]);                                         \
                }                                                              \
                free(O_FRAMES);                                                \
                O_FRAMES = NULL;                                               \
                break;                                                         \
            }                                                                  \
            for (size_t i = 0; i < FRAMECOUNT; ++i)                            \
            {                                                                  \
                O_FRAMES[ch][i] = I_FRAMES[(i * CHANNELS) + ch];               \
            }                                                                  \
        }                                                                      \
    } while (0)

// -----------------------------------------------------------------------------

wav_audio *wav_loadb(size_t len, uint8_t *data)
{
#define POPBYTES(VALUE, LEN)                                                   \
    do                                                                         \
    {                                                                          \
        memcpy(&(VALUE), &(data)[i], (LEN));                                   \
        i += (LEN);                                                            \
    } while (0)

#define GETBYTES(BYTES, LEN)                                                   \
    do                                                                         \
    {                                                                          \
        BYTES = malloc(LEN);                                                   \
        memcpy((BYTES), &(data)[i], (LEN));                                    \
        i += (LEN);                                                            \
    } while (0)

    if (len < 44)
    {
        fprintf(stderr, "ERROR: WAV header too small\n");
        return NULL;
    }
    wav_audio *wav = malloc(sizeof(wav_audio));

    char *riffHeader;
    uint32_t fileSizeMinusEight;
    char *waveHeader;
    char *fmtHeader;
    uint32_t formatChunkSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char *dataHeader;
    uint32_t dataLength;

    size_t i = 0;
    GETBYTES(riffHeader, 4);
    POPBYTES(fileSizeMinusEight, 4);
    GETBYTES(waveHeader, 4);
    GETBYTES(fmtHeader, 4);

    if (strncmp(riffHeader, "RIFF", 4))
    {
        fprintf(stderr, "ERROR: Invalid RIFF header '%.4s'\n", riffHeader);
        goto error;
    }
    if (fileSizeMinusEight + 8 != len)
    {
        fprintf(stderr,
                "ERROR: Invalid chunk size '%u' (%lld)\n",
                fileSizeMinusEight,
                (ptrdiff_t) len - (ptrdiff_t) fileSizeMinusEight);
        goto error;
    }
    if (strncmp(waveHeader, "WAVE", 4))
    {
        fprintf(stderr, "ERROR: Invalid WAVE header '%.4s'\n", waveHeader);
        goto error;
    }
    if (strncmp(fmtHeader, "fmt ", 4))
    {
        fprintf(stderr, "ERROR: Invalid format header '%.4s'\n", fmtHeader);
        goto error;
    }

    POPBYTES(formatChunkSize, 4);
    POPBYTES(audioFormat, 2);
    POPBYTES(numChannels, 2);
    POPBYTES(sampleRate, 4);
    POPBYTES(byteRate, 4);
    POPBYTES(blockAlign, 2);
    POPBYTES(bitsPerSample, 2);
    GETBYTES(dataHeader, 4);

    if (strncmp(dataHeader, "data", 4))
    {
        fprintf(stderr, "ERROR: Invalid data header '%.4s'\n", dataHeader);
        FILE *file = fopen("DUMP.wav", "wb");
        fwrite(data, 1, i, file);
        fclose(file);
        goto error;
    }

    POPBYTES(dataLength, 4);

    wav->channels    = numChannels;
    wav->sampleRate  = sampleRate;
    wav->sampleCount = (8 * dataLength) / (bitsPerSample * numChannels);

    if (audioFormat == wav_pcm)
    {
        if (bitsPerSample == 16)
        {
            wav->format  = wav_int16;
            size_t blen  = wav->channels * wav->sampleCount * sizeof(int16_t);
            int16_t *raw = malloc(blen);
            GETBYTES(raw, blen);
            WAV_DEINTERLEAVE(raw,
                             int16_t,
                             wav->int16,
                             int16_t,
                             wav->channels,
                             wav->sampleCount);
            free(raw);
        }
        else if (bitsPerSample == 32)
        {
            wav->format  = wav_int32;
            size_t blen  = wav->channels * wav->sampleCount * sizeof(int32_t);
            int32_t *raw = malloc(blen);
            GETBYTES(raw, blen);
            WAV_DEINTERLEAVE(raw,
                             int32_t,
                             wav->int32,
                             int32_t,
                             wav->channels,
                             wav->sampleCount);
            free(raw);
        }
        else
        {
            fprintf(
                stderr, "ERROR: Bit width '%u' not supported\n", bitsPerSample);
            goto error;
        }
    }
    else if (audioFormat == wav_float)
    {
        wav->format = wav_float32;
        size_t blen = wav->channels * wav->sampleCount * sizeof(float);
        float *raw  = malloc(blen);
        GETBYTES(raw, blen);
        WAV_DEINTERLEAVE(
            raw, float, wav->float32, float, wav->channels, wav->sampleCount);
        free(raw);
    }
    else
    {
        fprintf(stderr, "ERROR: WAV format not supported\n");
        goto error;
    }

    return wav;

error:
    free(wav);
    return NULL;

#undef GETBYTES
#undef POPBYTES
}

// -----------------------------------------------------------------------------

wav_audio *wav_loadf(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "ERROR: Could not read file '%s'\n", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);
    uint8_t *data = (uint8_t *) malloc(len);
    if (data == NULL)
    {
        fclose(file);
        return NULL;
    }
    size_t bytesRead = fread(data, 1, len, file);
    if (bytesRead != len)
    {
        fprintf(stderr, "ERROR: Could not read entire file '%s'\n", filename);
        free(data);
        fclose(file);
        return NULL;
    }
    wav_audio *wav = wav_loadb(len, data);
    fclose(file);
    return wav;
}

// -----------------------------------------------------------------------------

uint8_t *wav_dumpb(const wav_audio *wav, size_t *len)
{
#define PSHVAL(VALUE, LEN)                                                     \
    do                                                                         \
    {                                                                          \
        memcpy(&(data)[i], &(VALUE), (LEN));                                   \
        i += (LEN);                                                            \
    } while (0)

#define CPYVAL(BYTES, LEN)                                                     \
    do                                                                         \
    {                                                                          \
        memcpy(&(data)[i], (BYTES), (LEN));                                    \
        i += (LEN);                                                            \
    } while (0)

    uint32_t dataLength = 0;
    uint8_t bitWidth    = 0;
    switch (wav->format)
    {
        case wav_int16:
            dataLength = wav->channels * wav->sampleCount * sizeof(int16_t);
            bitWidth   = sizeof(int16_t) * 8;
            break;
        case wav_int32:
            dataLength = wav->channels * wav->sampleCount * sizeof(int32_t);
            bitWidth   = sizeof(int32_t) * 8;
            break;
        case wav_float32:
            dataLength = wav->channels * wav->sampleCount * sizeof(float);
            bitWidth   = sizeof(float) * 8;
            break;
        default:
            fprintf(stderr, "ERROR: Invalid data format\n");
            return NULL;
    }

    uint8_t *data = (uint8_t *) malloc(sizeof(uint8_t) * (44 + dataLength));
    if (!data)
    {
        fprintf(stderr, "ERROR: Memory allocation failure\n");
        *len = 0;
        return NULL;
    }

    size_t i                    = 0;
    const char *riffHeader      = "RIFF";
    uint32_t fileSizeMinusEight = 36 + dataLength;
    const char *waveHeader      = "WAVE";
    const char *fmtHeader       = "fmt ";
    uint32_t formatChunkSize    = wav->format == wav_float32 ? 18 : 16;
    uint16_t audioFormat   = wav->format == wav_float32 ? wav_float : wav_pcm;
    uint16_t numChannels   = wav->channels;
    uint32_t sampleRate    = wav->sampleRate;
    uint32_t byteRate      = sampleRate * numChannels * (bitWidth / 8);
    uint16_t blockAlign    = numChannels * (bitWidth / 8);
    uint16_t bitsPerSample = bitWidth;
    const char *dataHeader = "data";
    CPYVAL(riffHeader, 4);
    PSHVAL(fileSizeMinusEight, 4);
    CPYVAL(waveHeader, 4);
    CPYVAL(fmtHeader, 4);
    PSHVAL(formatChunkSize, 4);
    PSHVAL(audioFormat, 2);
    PSHVAL(numChannels, 2);
    PSHVAL(sampleRate, 4);
    PSHVAL(byteRate, 4);
    PSHVAL(blockAlign, 2);
    PSHVAL(bitsPerSample, 2);
    CPYVAL(dataHeader, 4);
    PSHVAL(dataLength, 4);

    switch (wav->format)
    {
        case wav_int16:
        {
            int16_t *raw = NULL;
            WAV_INTERLEAVE(wav->int16,
                           int16_t,
                           raw,
                           int16_t,
                           wav->channels,
                           wav->sampleCount);
            CPYVAL(raw, dataLength);
            free(raw);
            break;
        }
        case wav_int32:
        {
            int32_t *raw = NULL;
            WAV_INTERLEAVE(wav->int32,
                           int32_t,
                           raw,
                           int32_t,
                           wav->channels,
                           wav->sampleCount);
            CPYVAL(raw, dataLength);
            free(raw);
            break;
        }
        case wav_float32:
        {
            float *raw = NULL;
            WAV_INTERLEAVE(wav->float32,
                           float,
                           raw,
                           float,
                           wav->channels,
                           wav->sampleCount);
            CPYVAL(raw, dataLength);
            free(raw);
            break;
        }
        default:
            break;
    }

    if (len)
    {
        *len = 44 + dataLength;
    }

    return data;

#undef CPYVAL
#undef PSHVAL
}

// -----------------------------------------------------------------------------

bool wav_dumpf(const char *filename, const wav_audio *wav)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        fprintf(stderr, "ERROR: Could not write to file '%s'\n", filename);
        return false;
    }
    size_t len;
    uint8_t *data = wav_dumpb(wav, &len);
    fwrite(data, sizeof(uint8_t), len, file);
    fclose(file);
    return true;
}

// -----------------------------------------------------------------------------

void wav_free(wav_audio *wav)
{
    (void) wav;
    switch (wav->format)
    {
        case wav_int16:
        {
            for (uint16_t c = 0; wav->channels; ++c)
            {
                free(wav->int32[c]);
            }
            free(wav->int32);
            break;
        }
        case wav_int32:
        {
            for (uint16_t c = 0; wav->channels; ++c)
            {
                free(wav->int32[c]);
            }
            free(wav->int32);
            break;
        }
        case wav_float32:
        {
            for (uint16_t c = 0; wav->channels; ++c)
            {
                free(wav->float32[c]);
            }
            free(wav->float32);
            break;
        }
    }
}

// -----------------------------------------------------------------------------

#endif