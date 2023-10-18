#!/usr/bin/env bash
set -xe

OS=Undefined
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    OS=Linux
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    OS=Windows
fi

SDIR="./src"
SRC=(fileio.c audio.c pluxked.c)
for ((i = 0; i < ${#SRC[@]}; i++)); do
    SRC[$i]="$SDIR/${SRC[$i]}"
done

IDIR="./include"
LDIR="./lib"
LIBS=""
CFLAGS="-Wall -Wextra -Werror -Wno-missing-braces -ggdb"
ODIR="-o out/"
OFILE="pluxked"

if [[ $OS == "Windows" ]]; then
    OFILE="$OFILE.exe"
    LIBS="$LIBS -lShlwapi"
elif [[ $OS == "Linux" ]]; then
    LIBS="$LIBS -lm"
fi

mkdir -p out
mkdir -p res
clang $CFLAGS -I$IDIR -L$LDIR $LIBS $ODIR$OFILE "${SRC[@]}"

cd out
cp -r ../res/ ./
exec ./$OFILE res/test.wav res/test2.wav
