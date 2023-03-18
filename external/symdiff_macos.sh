#!/bin/bash
set -e
set -u
# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/
for TYPE in debug release; do
    NAME=osx_${TYPE}
    mkdir -p ${NAME}
    (cd $NAME; ${CMAKE} \
        ${ARCH_ARG} \
        -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
        -DCMAKE_BUILD_TYPE=${TYPE} \
        -DCMAKE_CXX_COMPILER=${CXX} \
        -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
        -DPYTHON3=ON \
        -DTCLMAIN=OFF \
        ..)
done

