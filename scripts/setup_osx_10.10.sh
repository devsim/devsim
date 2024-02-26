#!/bin/bash
set -e
set -u
DEVSIM_CONFIG="osx_10.10"
for TYPE in debug release; do
  for ARCH in x86_64; do
  #for ARCH in i386 x86_64; do
    NAME=osx_${ARCH}_${TYPE}
    mkdir -p ${NAME}
    (cd $NAME; cmake \
      ${ARCH_ARG} \
      -DCMAKE_BUILD_TYPE=${TYPE} \
      -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_C_COMPILER=${CC} \
      -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
      -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
      -DDEVSIM_EXTENDED_PRECISION=ON \
      -DDEVSIM_CPP_BIN_FLOAT=ON \
      -DEIGEN_DENSE_EXTENDED=ON \
      -DMKL_PARDISO=ON \
      -DPYTHON3=ON \
      -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
      ..)
  done
done
