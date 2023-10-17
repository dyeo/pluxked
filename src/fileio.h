#ifndef _FILEIO_H
#define _FILEIO_H

#include "plx.h"

bool fexists(const char *filename);
char *fmimetype(const char *filepath);

#endif