# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# Note: run this file using bash

DEVSIM_CONFIG="msys"
CMAKE_CXX_FLAGS=""
GENERATOR="MSYS Makefiles"
CXX=g++
CC=gcc
for TYPE in debug release; do
ARCH=`uname -m`
    NAME=msys_${ARCH}_${TYPE}
    mkdir ${NAME}
    (cd $NAME; cmake \
      -G "${GENERATOR}" \
      -DCMAKE_BUILD_TYPE=${TYPE} \
      -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_C_COMPILER=${CC} \
      -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
      -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
      -DDEVSIM_EXTENDED_PRECISION=ON \
      -DMKL_PARDISO=ON \
      -DPYTHON3=ON \
      -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
      -DPYTHON3_ARCHIVE=${PYTHON3_ARCHIVE} \
      -DCONDA_PREFIX=${CONDA_PREFIX} \
      ..)
#  done
done

