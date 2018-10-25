#!/bin/bash
mkdir -p build
cd build
cmake -DCMAKE_Fortran_FLAGS="-freal-8-real-16 -ffixed-line-length-0 -fPIC" -DCMAKE_BUILD_TYPE=RELEASE ..
make -j2

