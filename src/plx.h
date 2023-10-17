#ifndef _PLX_H
#define _PLX_H

#if defined(_WIN32) || defined(_WIN64)
#define PLX_WINDOWS 1
#define PLX_POSIX 0
#else
#define PLX_WINDOWS 0
#define PLX_POSIX 1
#endif

#if PLX_WINDOWS
#define _CRT_SECURE_NO_WARNINGS 1
#elif PLX_POSIX
#else
#error Unknown platform.
#endif

#include <assert.h>
#include <float.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

static inline i32 ftoiss(const f32 f, const f32 scale)
{
    return (i32) ((f / scale) * (f32) INT_MAX);
}

static inline f32 itofss(const i32 i, const f32 scale)
{
    return ((((f32) i) / (f32) INT_MAX) * scale);
}

static inline i32 ftois(const f32 f)
{
    return ftoiss(f, FLT_MAX);
}

static inline f32 itofs(const i32 i)
{
    return itofss(i, FLT_MAX);
}

#endif
