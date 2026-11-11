#!/bin/bash
set -e
set -u
# this is verbose remove
set -x

export CMAKE="cmake"
export CMAKE_CXX_FLAGS=""
export CC=clang;
export CXX=clang++;
export F77="";
export ARCH_ARG="-DCMAKE_OSX_ARCHITECTURES=arm64"
export PLAT_NAME="arm64"
#FULL_PLAT_NAME=${PLAT_NAME}
export PYTHON3_BIN=/opt/homebrew/bin/python3
export PIP_BIN=pip

${PYTHON3_BIN} -mvenv venv
source venv/bin/activate

#export MACOSX_DEPLOYMENT_TARGET=12.0

${PIP_BIN} install wheel
export PYTHON3_INCLUDE=$(${PYTHON3_BIN} -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

#For macOS, the Xcode command line developer tools should be installed, these contain all the necessary libraries.  The math libraries are from the Apple Accelerate Framework.  Note that a FORTRAN compiler is not required.
#https://developer.apple.com/technologies/tools
#https://developer.apple.com/performance/accelerateframework.html

# SYMDIFF build
(cd external/symdiff && bash  ../symdiff_macos.sh && cd osx_release && make -j3)

mkdir -p build/symdiff
${CMAKE} \
    ${ARCH_ARG} \
    -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -DCMAKE_CXX_COMPILER=${CXX} \
    -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
    -DPYTHON3=ON \
    -DTCLMAIN=OFF \
    -B build/symdiff \
    -S external/symdiff

(cd build/symdiff && ${CMAKE} --build . -- -j2)

# umfpack support
mkdir -p build/umfpack_lgpl
${CMAKE} \
    ${ARCH_ARG} \
    -DCMAKE_C_COMPILER=${CC} \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -B build/umfpack_lgpl \
    -S external/umfpack_lgpl/umfpack

(cd build/umfpack_lgpl && ${CMAKE} --build . -- -j2)

DEVSIM_CONFIG="osx_10.10"
mkdir -p build/devsim
${CMAKE} \
    ${ARCH_ARG} \
    -DCMAKE_BUILD_TYPE=RELEASE \
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
    -B build/devsim \
    -S .
(cd build/devsim && ${CMAKE} --build . -- -j2)

DIST_NAME=devsim_macos_${PLAT_NAME}_${1}
(cd dist && bash package_macos.sh ${DIST_NAME});
cp -f dist/bdist_wheel/setup.* dist/${DIST_NAME}
#echo PACKAGING $FULL_PLAT_NAME
#if [[ -n "$FULL_PLAT_NAME" ]]; then
#(cd dist/${DIST_NAME} &&  perl -p -i -e "s/^#plat-name.*/plat-name = ${FULL_PLAT_NAME}/" setup.cfg);
#fi
(cd dist/${DIST_NAME} && ${PIP_BIN} wheel .)
(cp dist/${DIST_NAME}/*.whl dist)
