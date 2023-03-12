#!/bin/bash
set -e
set -u
DEVSIM_CONFIG="osx_gcc"
for TYPE in debug release; do
  for ARCH in x86_64; do
  #for ARCH in i386 x86_64; do
    NAME=osx_${ARCH}_${TYPE}
    mkdir ${NAME}
    (cd $NAME; cmake \
      -DCMAKE_BUILD_TYPE=${TYPE} \
      -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_C_COMPILER=${CC} \
      -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
      -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
      -DDEVSIM_EXTENDED_PRECISION=ON \
      -DMKL_PARDISO=ON \
      -DPYTHON3=ON \
      -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
      ..)
  done
done
