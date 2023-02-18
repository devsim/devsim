#!/bin/bash
set -e
set -u
mkdir -p msys
ANACONDA_PATH=${CONDA_PREFIX}
CMAKE=$(cygpath -w ${ANACONDA_PATH}/Library/bin/cmake.exe)
GENERATOR="MSYS Makefiles"
CXX=g++
F77=gfortran
(cd msys && $CMAKE -G "${GENERATOR}" -DCMAKE_CXX_COMPILER=${CXX} -DCMAKE_Fortran_COMPILER=${F77} -DCMAKE_Fortran_FLAGS="-freal-8-real-16 -ffixed-line-length-0" -DCMAKE_BUILD_TYPE=RELEASE ..)
