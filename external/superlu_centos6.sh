#!/bin/sh
make CC=/opt/rh/devtoolset-6/root/usr/bin/gcc SuperLUroot=`pwd` SUPERLULIB=`pwd`/lib/libsuperlu_4.3.a NOOPTS="-fPIC" CFLAGS="-O3 -DPRNTlevel=0 -fPIC" CDEFS="-Dadd_" superlulib
\rm SRC/*.o
