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
FULL_PLAT_NAME=${PLAT_NAME}
#  export ARCH_ARG="-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64"
#  export PLAT_NAME="universal2"
export PYTHON3_BIN=/usr/bin/python3
export PIP_BIN=/usr/bin/pip3

export MACOSX_DEPLOYMENT_TARGET=12.0

${PIP_BIN} install wheel
export PYTHON3_INCLUDE=$(${PYTHON3_BIN} -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

#For macOS, the Xcode command line developer tools should be installed, these contain all the necessary libraries.  The math libraries are from the Apple Accelerate Framework.  Note that a FORTRAN compiler is not required.
#https://developer.apple.com/technologies/tools
#https://developer.apple.com/performance/accelerateframework.html

# SYMDIFF build
(cd external/symdiff && bash  ../symdiff_macos.sh && cd osx_release && make -j3)

# umfpack support
(cd external/umfpack_lgpl && bash setup_macos.sh && cd build && make -j3)

bash ./scripts/setup_osx_10.10.sh

(cd osx_x86_64_release && make -j3)
DIST_NAME=devsim_macos_${PLAT_NAME}_${2}
(cd dist && bash package_macos.sh ${1} ${DIST_NAME});
cp -f dist/bdist_wheel/setup.* dist/${DIST_NAME}
echo PACKAGING $FULL_PLAT_NAME
if [[ -n "$FULL_PLAT_NAME" ]]; then
(cd dist/${DIST_NAME} &&  perl -p -i -e "s/^#plat-name.*/plat-name = ${FULL_PLAT_NAME}/" setup.cfg);
fi
(cd dist/${DIST_NAME} && ${PIP_BIN} wheel .)
(cp dist/${DIST_NAME}/*.whl dist)
