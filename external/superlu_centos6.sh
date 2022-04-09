#!/bin/sh
make CC=gcc SuperLUroot=`pwd` SUPERLULIB=`pwd`/lib/libsuperlu_4.3.a NOOPTS="-fPIC" CFLAGS="-g -DPRNTlevel=0 -fPIC" CDEFS="-Dadd_" superlulib
\rm SRC/*.o
