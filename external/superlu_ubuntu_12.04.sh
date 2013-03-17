#!/bin/sh
make CC=/usr/bin/gcc SuperLUroot=`pwd` SUPERLULIB=`pwd`/lib/libsuperlu_4.3.a NOOPTS="-m64" CFLAGS="-m64 -O3 -DPRNTlevel=0" CDEFS="-Dadd_" superlulib
\rm SRC/*.o
