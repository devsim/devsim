#!/bin/bash

DEVSIM_CONFIG="fedora_27"
CMAKE="/usr/bin/cmake3"
CMAKE_CXX_FLAGS=""
CXX="/usr/bin/g++"
CC="/usr/bin/gcc"
for TYPE in debug release; do
ARCH=`uname -m`
#  for ARCH in i386 x86_64; do
    NAME=linux_${ARCH}_${TYPE}
    mkdir -p ${NAME}
    (cd $NAME; ${CMAKE} \
      -DCMAKE_BUILD_TYPE=${TYPE} \
      -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_C_COMPILER=${CC} \
      -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
      -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
      -DDEVSIM_EXTENDED_PRECISION=ON \
      ..)
#  done
done
