#!/bin/sh
make CC=${CC} SuperLUroot=`pwd` SUPERLULIB=`pwd`/lib/libsuperlu_4.3.a NOOPTS="-fPIC" CFLAGS="-O3 -DPRNTlevel=0 -fPIC" CDEFS="-Dadd_" superlulib
\rm SRC/*.o
