#!/bin/bash
set -e
set -u
mkdir -p build
(cd build && ${CMAKE} -DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CXX} -DCMAKE_Fortran_COMPILER=${F77} -DCMAKE_Fortran_FLAGS="-freal-8-real-16 -ffixed-line-length-0 -fPIC" -DCMAKE_BUILD_TYPE=RELEASE ..)
