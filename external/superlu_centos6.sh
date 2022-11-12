#!/bin/sh
set -e
mkdir -p build
cd build
${CMAKE} -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_C_COMPILER=${CC} -Denable_complex=OFF -Denable_single=OFF -Denable_doc=OFF -Denable_tests=OFF -DXSDK_ENABLE_Fortran=OFF -DBLAS_FOUND=ON -DCMAKE_C_FLAGS="-fpic" ..
make -j2
