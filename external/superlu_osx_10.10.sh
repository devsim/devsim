#!/bin/sh
#make CC=/usr/bin/gcc SuperLUroot=`pwd` SUPERLULIB=`pwd`/lib/libsuperlu_4.3_i386.a NOOPTS="-m32" CFLAGS="-m32 -O2 -DPRNTlevel=0" CDEFS="-Dadd_" superlulib
set -e
\rm -f SRC/*.o
echo $CC
if [ -z "${CC}" ]; then
export CC=/usr/bin/gcc
fi
echo $CC
make CC=${CC} SuperLUroot=`pwd` SUPERLULIB=`pwd`/lib/libsuperlu_4.3_x86_64.a NOOPTS="-m64" CFLAGS="-m64 -O3 -fvisibility=hidden -DPRNTlevel=0" CDEFS="-Dadd_" superlulib
\rm -f SRC/*.o
