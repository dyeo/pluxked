#ifndef _WAV_H
#define _WAV_H

#if defined(_WIN32) || defined(_WIN64)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------

typedef enum wav_sampleformat {
  wav_pcm16,
  wav_pcm32,
  wav_float32,
} wav_sampleformat;

typedef struct wav_audio {
  wav_sampleformat format;
  uint16_t channels;
  uint32_t sampleRate;
  uint32_t sampleCount;
  union {
    int16_t **pcm16;
    int32_t **pcm32;
    float **float32;
  };
} wav_audio;

#define WAV_AUDIO_FOREACH(AUDIO, C, S)                                         \
  (uint32_t C = 0, S = 0; S < (AUDIO)->sampleCount;                            \
   S += C, C++, C %= (AUDIO)->channels)

// -----------------------------------------------------------------------------

extern int16_t wav_pcm32_to_pcm16(int32_t sample);
extern int16_t wav_float32_to_pcm16(float sample);
extern int32_t wav_pcm16_to_pcm32(int16_t sample);
extern int32_t wav_float32_to_pcm32(float sample);
extern float wav_pcm16_to_float32(int16_t sample);
extern float wav_pcm32_to_float32(int32_t sample);
extern void wav_to_pcm16(wav_audio *wav);
extern float wav_pcm32_to_float32(int32_t sample);
extern void wav_to_pcm16(wav_audio *wav);
extern void wav_to_pcm32(wav_audio *wav);
extern void wav_to_float32(wav_audio *wav);

extern wav_audio *wav_loadf(const char *filename);
extern bool wav_dumpf(const char *filename, const wav_audio *wav);

extern wav_audio *wav_loadb(size_t len, uint8_t *data);
extern uint8_t *wav_dumpb(const wav_audio *wav, size_t *len);

extern void wav_free(wav_audio *wav);

// -----------------------------------------------------------------------------

#define wav_invalid 0

typedef enum wav_audioformat {
  wav_pcm = 0x0001,
  wav_adpcm = 0x0002,
  wav_float = 0x0003,
  wav_extended = 0xFFFE
} wav_audioformat;

typedef enum wav_samplerate {
  wav_8000hz = 8000,
  wav_11025hz = 11025,
  wav_16000hz = 16000,
  wav_22050hz = 22050,
  wav_32000hz = 32000,
  wav_44100hz = 44100,
  wav_48000hz = 48000,
  wav_88200hz = 88200,
  wav_96000hz = 96000,
  wav_176400hz = 176400,
  wav_192000hz = 192000,
  wav_384000hz = 384000
} wav_samplerate;

typedef enum plx_wav_bitspersample {
  plx_wav_16bit = 16,
  plx_wav_24bit = 24,
  plx_wav_32bit = 32,
} plx_wav_bitspersample;

// -----------------------------------------------------------------------------

#endif

#ifdef WAV_IMPLEMENTATION
#define WAV_IMPLEMENTATION

// -----------------------------------------------------------------------------

#define WAV_INTERLEAVE(I_FRAMES, I_TYPE, O_FRAMES, O_TYPE, CHANNELS,           \
                       FRAMECOUNT)                                             \
  do {                                                                         \
    O_FRAMES = (O_TYPE *)malloc(sizeof(O_TYPE) * CHANNELS * FRAMECOUNT);       \
    if (O_FRAMES == NULL)                                                      \
      break;                                                                   \
    for (size_t i = 0; i < FRAMECOUNT; ++i) {                                  \
      for (size_t ch = 0; ch < CHANNELS; ++ch) {                               \
        (O_FRAMES)[(i * CHANNELS) + ch] = (I_FRAMES)[ch][i];                   \
      }                                                                        \
    }                                                                          \
  } while (0)

#define WAV_DEINTERLEAVE(I_FRAMES, I_TYPE, O_FRAMES, O_TYPE, CHANNELS,         \
                         FRAMECOUNT)                                           \
  do {                                                                         \
    O_FRAMES = (O_TYPE **)malloc(sizeof(O_TYPE *) * CHANNELS);                 \
    if (O_FRAMES == NULL)                                                      \
      break;                                                                   \
    for (size_t ch = 0; ch < CHANNELS; ++ch) {                                 \
      (O_FRAMES)[ch] = (O_TYPE *)malloc(sizeof(O_TYPE) * FRAMECOUNT);          \
      if ((O_FRAMES)[ch] == NULL) {                                            \
        for (size_t j = 0; j < ch; ++j) {                                      \
          free((O_FRAMES)[j]);                                                 \
        }                                                                      \
        free(O_FRAMES);                                                        \
        O_FRAMES = NULL;                                                       \
        break;                                                                 \
      }                                                                        \
      for (size_t i = 0; i < FRAMECOUNT; ++i) {                                \
        (O_FRAMES)[ch][i] = (I_FRAMES)[(i * CHANNELS) + ch];                   \
      }                                                                        \
    }                                                                          \
  } while (0)

#define WAV_POPVAL(VALUE)                                                      \
  do {                                                                         \
    memcpy(&(VALUE), &(data)[i], sizeof(VALUE));                               \
    i += sizeof(VALUE);                                                        \
  } while (0)

#define WAV_PUSHVAL(VALUE)                                                     \
  do {                                                                         \
    memcpy(&(data)[i], &(VALUE), sizeof(VALUE));                               \
    i += sizeof(VALUE);                                                        \
  } while (0)

#define WAV_POPBYTES(BYTES, LEN)                                               \
  do {                                                                         \
    BYTES = malloc(LEN);                                                       \
    memcpy((BYTES), &(data)[i], (LEN));                                        \
    i += (LEN);                                                                \
  } while (0)

#define WAV_PUSHBYTES(BYTES, LEN)                                              \
  do {                                                                         \
    memcpy(&(data)[i], (BYTES), (LEN));                                        \
    i += (LEN);                                                                \
  } while (0)

// -----------------------------------------------------------------------------

int16_t wav_pcm32_to_pcm16(int32_t sample) { return (int16_t)(sample >> 16); }

int16_t wav_float32_to_pcm16(float sample) {
  return (int16_t)(sample * ((float)0x7FFF));
}

float wav_pcm16_to_float32(int16_t sample) { return sample / (float)0x7FFF; }

float wav_pcm32_to_float32(int32_t sample) {
  return sample / (float)0x7FFFFFFF;
}

int32_t wav_pcm16_to_pcm32(int16_t sample) { return ((int32_t)sample) << 16; }

int32_t wav_float32_to_pcm32(float sample) {
  return (int32_t)(sample * ((float)0x7FFFFFFF));
}

// -----------------------------------------------------------------------------

typedef int16_t (*wav_gen_pcm16_callback)(uint32_t channels,
                                          uint32_t sampleRate,
                                          uint32_t sampleCount,
                                          int32_t channelIndex,
                                          int32_t sampleIndex);

wav_audio *wav_gen_pcm16(uint32_t channels, uint32_t sampleRate,
                         uint32_t sampleCount,
                         wav_gen_pcm16_callback callback) {
  if (channels == 0 || sampleRate == 0 || sampleCount == 0 ||
      callback == NULL) {
    return NULL;
  }
  wav_audio *wav = malloc(sizeof(wav_audio));
  wav->format = wav_pcm16;
  wav->channels = channels;
  wav->sampleRate = sampleRate;
  wav->sampleCount = sampleCount;
  wav->pcm16 = malloc(sizeof(int16_t *));
  for (uint32_t c = 0; c < channels; ++c) {
    wav->pcm32[c] = malloc(sampleCount * sizeof(int16_t));
    for (uint32_t i = 0; i < sampleCount; ++i) {
      wav->pcm16[c][i] = callback(channels, sampleRate, sampleCount, c, i);
    }
  }
  return wav;
}

typedef int32_t (*wav_gen_pcm32_callback)(uint32_t channels,
                                          uint32_t sampleRate,
                                          uint32_t sampleCount,
                                          int32_t channelIndex,
                                          int32_t sampleIndex);

wav_audio *wav_gen_pcm32(uint32_t channels, uint32_t sampleRate,
                         uint32_t sampleCount,
                         wav_gen_pcm32_callback callback) {
  if (channels == 0 || sampleRate == 0 || sampleCount == 0 ||
      callback == NULL) {
    return NULL;
  }
  wav_audio *wav = malloc(sizeof(wav_audio));
  wav->format = wav_pcm32;
  wav->channels = channels;
  wav->sampleRate = sampleRate;
  wav->sampleCount = sampleCount;
  wav->pcm32 = malloc(sizeof(int32_t *));
  for (uint32_t c = 0; c < channels; ++c) {
    wav->pcm32[c] = malloc(sampleCount * sizeof(int32_t));
    for (uint32_t i = 0; i < sampleCount; ++i) {
      wav->pcm32[c][i] = callback(channels, sampleRate, sampleCount, c, i);
    }
  }
  return wav;
}

typedef float (*wav_gen_float32_callback)(uint32_t channels,
                                          uint32_t sampleRate,
                                          uint32_t sampleCount,
                                          int32_t channelIndex,
                                          int32_t sampleIndex);

wav_audio *wav_gen_float32(uint32_t channels, uint32_t sampleRate,
                           uint32_t sampleCount,
                           wav_gen_float32_callback callback) {
  if (channels == 0 || sampleRate == 0 || sampleCount == 0 ||
      callback == NULL) {
    return NULL;
  }
  wav_audio *wav = malloc(sizeof(wav_audio));
  wav->format = wav_float32;
  wav->channels = channels;
  wav->sampleRate = sampleRate;
  wav->sampleCount = sampleCount;
  wav->float32 = malloc(sizeof(float *));
  for (uint32_t c = 0; c < channels; ++c) {
    wav->pcm32[c] = malloc(sampleCount * sizeof(int32_t));
    for (uint32_t i = 0; i < sampleCount; ++i) {
      wav->float32[c][i] = callback(channels, sampleRate, sampleCount, c, i);
    }
  }
  return wav;
}

// -----------------------------------------------------------------------------

void wav_to_pcm16(wav_audio *wav) {
  if (!wav)
    return;
  int16_t **new_data = malloc(wav->channels * sizeof(int16_t *));
  if (new_data == NULL) {
    fprintf(stderr, "ERROR: Memory allocation error\n");
    free(new_data);
    return;
  }
  for (uint16_t c = 0; c < wav->channels; c++) {
    new_data[c] = malloc(wav->sampleCount * sizeof(int16_t));
    for (uint32_t i = 0; i < wav->sampleCount; i++) {
      switch (wav->format) {
      case wav_pcm32:
        new_data[c][i] = wav_pcm32_to_pcm16(wav->pcm32[c][i]);
        break;
      case wav_float32:
        new_data[c][i] = wav_float32_to_pcm16(wav->float32[c][i]);
        break;
      default:
        return;
      }
    }
  }
  for (uint16_t ch = 0; ch < wav->channels; ch++) {
    free(wav->pcm16[ch]);
  }
  free(wav->pcm16);
  wav->pcm16 = new_data;
  wav->format = wav_pcm16;
}

void wav_to_pcm32(wav_audio *wav) {
  if (!wav || wav->format == wav_pcm32)
    return;
  int32_t **new_data = malloc(wav->channels * sizeof(int32_t *));
  if (new_data == NULL) {
    fprintf(stderr, "ERROR: Memory allocation error\n");
    free(new_data);
    return;
  }
  switch (wav->format) {
  default:
    return;
  case wav_pcm16: {
    for
      WAV_AUDIO_FOREACH(wav, c, i) {
        new_data[c][i] = wav_pcm16_to_pcm32(wav->pcm16[c][i]);
      }
  } break;
  case wav_pcm32: {
    for
      WAV_AUDIO_FOREACH(wav, c, i) {
        new_data[c][i] = wav_float32_to_pcm32(wav->pcm32[c][i]);
      }
  } break;
  }
  for (uint16_t ch = 0; ch < wav->channels; ch++) {
    free(wav->pcm32[ch]);
  }
  free(wav->pcm32);
  wav->pcm32 = new_data;
  wav->format = wav_pcm32;
}

void wav_to_float32(wav_audio *wav) {
  if (!wav || wav->format == wav_float32)
    return;
  float **new_data = malloc(wav->channels * sizeof(float *));
  if (new_data == NULL) {
    fprintf(stderr, "ERROR: Memory allocation error\n");
    free(new_data);
    return;
  }
  for (uint16_t c = 0; c < wav->channels; c++) {
    new_data[c] = malloc(wav->sampleCount * sizeof(float));
    for (uint32_t i = 0; i < wav->sampleCount; i++) {
      switch (wav->format) {
      case wav_pcm16:
        new_data[c][i] = wav_pcm16_to_float32(wav->pcm16[c][i]);
        break;
      case wav_pcm32:
        new_data[c][i] = wav_pcm32_to_float32(wav->pcm32[c][i]);
        break;
      default:
        return;
      }
    }
  }
  for (uint16_t ch = 0; ch < wav->channels; ch++) {
    free(wav->float32[ch]);
  }
  free(wav->float32);
  wav->float32 = new_data;
  wav->format = wav_float32;
}

// -----------------------------------------------------------------------------

wav_audio *wav_loadf(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    fprintf(stderr, "ERROR: Could not read file '%s'\n", filename);
    return NULL;
  }
  fseek(file, 0, SEEK_END);
  size_t len = ftell(file);
  fseek(file, 0, SEEK_SET);
  uint8_t *data = (uint8_t *)malloc(len);
  if (data == NULL) {
    fclose(file);
    return NULL;
  }
  size_t bytesRead = fread(data, 1, len, file);
  if (bytesRead != len) {
    fprintf(stderr, "ERROR: Could not read entire file '%s'\n", filename);
    free(data);
    fclose(file);
    return NULL;
  }
  wav_audio *wav = wav_loadb(len, data);
  fclose(file);
  return wav;
}

bool wav_dumpf(const char *filename, const wav_audio *wav) {
  FILE *file = fopen(filename, "wb");
  if (file == NULL) {
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

wav_audio *wav_loadb(size_t len, uint8_t *data) {

  if (len < 44) {
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
  WAV_POPBYTES(riffHeader, 4);
  WAV_POPVAL(fileSizeMinusEight);
  WAV_POPBYTES(waveHeader, 4);
  WAV_POPBYTES(fmtHeader, 4);

  if (strncmp(riffHeader, "RIFF", 4)) {
    fprintf(stderr, "ERROR: Invalid RIFF header '%.4s'\n", riffHeader);
    goto error;
  }
  if (fileSizeMinusEight + 8 != len) {
    fprintf(stderr, "ERROR: Invalid chunk size '%u' (%lld)\n",
            fileSizeMinusEight, (ptrdiff_t)len - (ptrdiff_t)fileSizeMinusEight);
    goto error;
  }
  if (strncmp(waveHeader, "WAVE", 4)) {
    fprintf(stderr, "ERROR: Invalid WAVE header '%.4s'\n", waveHeader);
    goto error;
  }
  if (strncmp(fmtHeader, "fmt ", 4)) {
    fprintf(stderr, "ERROR: Invalid format header '%.4s'\n", fmtHeader);
    goto error;
  }

  WAV_POPVAL(formatChunkSize);
  WAV_POPVAL(audioFormat);
  WAV_POPVAL(numChannels);
  WAV_POPVAL(sampleRate);
  WAV_POPVAL(byteRate);
  WAV_POPVAL(blockAlign);
  WAV_POPVAL(bitsPerSample);
  WAV_POPBYTES(dataHeader, 4);

  if (strncmp(dataHeader, "data", 4)) {
    fprintf(stderr, "ERROR: Invalid data header '%.4s'\n", dataHeader);
    FILE *file = fopen("DUMP.wav", "wb");
    fwrite(data, 1, i, file);
    fclose(file);
    goto error;
  }

  WAV_POPVAL(dataLength);

  wav->channels = numChannels;
  wav->sampleRate = sampleRate;
  wav->sampleCount = dataLength / ((bitsPerSample / 8) * numChannels);

  if (audioFormat == wav_pcm) {
    if (bitsPerSample == 16) {
      wav->format = wav_pcm16;
      size_t blen = wav->channels * wav->sampleCount * sizeof(int16_t);
      int16_t *raw = malloc(blen);
      WAV_POPBYTES(raw, blen);
      WAV_DEINTERLEAVE(raw, int16_t, wav->pcm16, int16_t, wav->channels,
                       wav->sampleCount);
      free(raw);
    } else if (bitsPerSample == 32) {
      wav->format = wav_pcm32;
      size_t blen = wav->channels * wav->sampleCount * sizeof(int32_t);
      int32_t *raw = malloc(blen);
      WAV_POPBYTES(raw, blen);
      WAV_DEINTERLEAVE(raw, int32_t, wav->pcm32, int32_t, wav->channels,
                       wav->sampleCount);
      free(raw);
    } else {
      fprintf(stderr, "ERROR: Bit width '%u' not supported\n", bitsPerSample);
      goto error;
    }
  } else if (audioFormat == wav_float) {
    wav->format = wav_float32;
    size_t blen = wav->channels * wav->sampleCount * sizeof(float);
    float *raw = malloc(blen);
    WAV_POPBYTES(raw, blen);
    WAV_DEINTERLEAVE(raw, float, wav->float32, float, wav->channels,
                     wav->sampleCount);
    free(raw);
  } else {
    fprintf(stderr, "ERROR: WAV format not supported\n");
    goto error;
  }

  return wav;

error:
  free(wav);
  return NULL;
}

uint8_t *wav_dumpb(const wav_audio *wav, size_t *len) {
  uint32_t dataLength = 0;
  uint8_t bitWidth = 0;
  switch (wav->format) {
  case wav_pcm16:
    dataLength = wav->channels * wav->sampleCount * sizeof(int16_t);
    bitWidth = sizeof(int16_t) * 8;
    break;
  case wav_pcm32:
    dataLength = wav->channels * wav->sampleCount * sizeof(int32_t);
    bitWidth = sizeof(int32_t) * 8;
    break;
  case wav_float32:
    dataLength = wav->channels * wav->sampleCount * sizeof(float);
    bitWidth = sizeof(float) * 8;
    break;
  default:
    fprintf(stderr, "ERROR: Invalid data format\n");
    return NULL;
  }

  uint8_t *data = (uint8_t *)malloc(sizeof(uint8_t) * (44 + dataLength));
  if (!data) {
    fprintf(stderr, "ERROR: Memory allocation failure\n");
    *len = 0;
    return NULL;
  }

  size_t i = 0;
  const char *riffHeader = "RIFF";
  uint32_t fileSizeMinusEight = 36 + dataLength;
  const char *waveHeader = "WAVE";
  const char *fmtHeader = "fmt ";
  uint32_t formatChunkSize = wav->format == wav_float32 ? 18 : 16;
  uint16_t audioFormat = wav->format == wav_float32 ? wav_float : wav_pcm;
  uint16_t numChannels = wav->channels;
  uint32_t sampleRate = wav->sampleRate;
  uint32_t byteRate = sampleRate * numChannels * (bitWidth / 8);
  uint16_t blockAlign = numChannels * (bitWidth / 8);
  uint16_t bitsPerSample = bitWidth;
  const char *dataHeader = "data";
  WAV_PUSHBYTES(riffHeader, 4);
  WAV_PUSHVAL(fileSizeMinusEight);
  WAV_PUSHBYTES(waveHeader, 4);
  WAV_PUSHBYTES(fmtHeader, 4);
  WAV_PUSHVAL(formatChunkSize);
  WAV_PUSHVAL(audioFormat);
  WAV_PUSHVAL(numChannels);
  WAV_PUSHVAL(sampleRate);
  WAV_PUSHVAL(byteRate);
  WAV_PUSHVAL(blockAlign);
  WAV_PUSHVAL(bitsPerSample);
  WAV_PUSHBYTES(dataHeader, 4);
  WAV_PUSHVAL(dataLength);

  switch (wav->format) {
  case wav_pcm16: {
    int16_t *raw = NULL;
    WAV_INTERLEAVE(wav->pcm16, int16_t, raw, int16_t, wav->channels,
                   wav->sampleCount);
    WAV_PUSHBYTES(raw, dataLength);
    free(raw);
    break;
  }
  case wav_pcm32: {
    int32_t *raw = NULL;
    WAV_INTERLEAVE(wav->pcm32, int32_t, raw, int32_t, wav->channels,
                   wav->sampleCount);
    WAV_PUSHBYTES(raw, dataLength);
    free(raw);
    break;
  }
  case wav_float32: {
    float *raw = NULL;
    WAV_INTERLEAVE(wav->float32, float, raw, float, wav->channels,
                   wav->sampleCount);
    WAV_PUSHBYTES(raw, dataLength);
    free(raw);
    break;
  }
  default:
    break;
  }

  if (len) {
    *len = i;
  }

  return data;
}

// -----------------------------------------------------------------------------

void wav_free(wav_audio *wav) {
  (void)wav;
  switch (wav->format) {
  case wav_pcm16: {
    for (uint16_t c = 0; wav->channels; ++c) {
      free(wav->pcm32[c]);
    }
    free(wav->pcm32);
    break;
  }
  case wav_pcm32: {
    for (uint16_t c = 0; wav->channels; ++c) {
      free(wav->pcm32[c]);
    }
    free(wav->pcm32);
    break;
  }
  case wav_float32: {
    for (uint16_t c = 0; wav->channels; ++c) {
      free(wav->float32[c]);
    }
    free(wav->float32);
    break;
  }
  }
}

#undef WAV_INTERLEAVE
#undef WAV_DEINTERLEAVE
#undef WAV_POPBYTES
#undef WAV_POPVAL
#undef WAV_PUSHBYTES
#undef WAV_PUSHVAL

// -----------------------------------------------------------------------------

#endif
