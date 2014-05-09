#!/bin/sh
make CC=icc SuperLUroot=`pwd` SUPERLULIB=`pwd`/lib/libsuperlu_4.3.a NOOPTS="" CFLAGS="-O3 -DPRNTlevel=0" CDEFS="-Dadd_" superlulib
\rm SRC/*.o
