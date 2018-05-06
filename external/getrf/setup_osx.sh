#!/bin/bash
mkdir -p build
(cd build && cmake -DCMAKE_CXX_COMPILER=/usr/local/bin/g++-8 -DCMAKE_Fortran_COMPILER=/usr/local/bin/gfortran -DCMAKE_Fortran_FLAGS="-freal-8-real-16 -ffixed-line-length-0" -DCMAKE_BUILD_TYPE=RELEASE ..)
