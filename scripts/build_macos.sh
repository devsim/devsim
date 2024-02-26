#!/bin/bash
set -e
set -u

if [ "${1}" = "gcc" ]
  then
  # try to fix build error
  #export HOMEBREW_NO_INSTALL_CLEANUP=1
  #brew update > /dev/null;
  #if brew ls --versions gcc > /dev/null;
  #then
  #brew outdated gcc || brew upgrade gcc;
  #else
  ## the overwrite is to fix the linking issue seen on travis ci
  #brew install gcc || brew link --overwrite gcc;
  #fi
  export CMAKE="cmake"
  export CMAKE_CXX_FLAGS=""
  export CC=/usr/local/bin/gcc-9;
  export CXX=/usr/local/bin/g++-9;
  export F77=/usr/local/bin/gfortran-9;
  export ARCH_ARG=""
  export PLAT_NAME="x86_64"
  export PYTHON3_BIN=/usr/local/bin/python3
  export PIP_BIN=/usr/local/bin/pip3
elif [ "${1}" = "clang" ]
  then
  export CMAKE="cmake"
  export CMAKE_CXX_FLAGS=""
  export CC=clang;
  export CXX=clang++;
  export F77="";
  export ARCH_ARG="-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64"
  export PLAT_NAME="universal2"
  export PYTHON3_BIN=python3
  export PIP_BIN=pip3
else
  echo "ERROR: FIRST ARGUMENT MUST BE gcc OR clang";
  exit 1;
fi

export MACOSX_DEPLOYMENT_TARGET=12.0

${PIP_BIN} install wheel
export PYTHON3_INCLUDE=$(${PYTHON3_BIN} -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

#For macOS, the Xcode command line developer tools should be installed, these contain all the necessary libraries.  The math libraries are from the Apple Accelerate Framework.  Note that a FORTRAN compiler is not required.
#https://developer.apple.com/technologies/tools
#https://developer.apple.com/performance/accelerateframework.html

# SYMDIFF build
if [ "${1}" = "gcc" ]
then
(cd external/symdiff && bash ../symdiff_macos.sh && cd osx_release && make -j4)
elif [ "${1}" = "clang" ]
then
(cd external/symdiff && bash  ../symdiff_macos.sh && cd osx_release && make -j4)
fi

# quad precision getrf
if [ "${1}" = "gcc" ]
then
(cd external/getrf && ./setup_osx.sh && cd build && make -j4)
fi

# umfpack support
(cd external/umfpack_lgpl && bash setup_macos.sh && cd build && make -j3)

if [ "${1}" = "gcc" ]
then
bash ./scripts/setup_osx_gcc.sh
elif [ "${1}" = "clang" ]
then
bash ./scripts/setup_osx_10.10.sh
fi

(cd osx_x86_64_release && make -j4)
DIST_NAME=devsim_macos_${PLAT_NAME}_${2}
(cd dist && bash package_macos.sh ${1} ${DIST_NAME});
cp -f dist/bdist_wheel/setup.* dist/${DIST_NAME}
FULL_PLAT_NAME=$(${PYTHON3_BIN} dist/bdist_wheel/fix_macos_arch.py ${PLAT_NAME})
echo PACKAGING $FULL_PLAT_NAME
if [[ -n "$FULL_PLAT_NAME" ]]; then
(cd dist/${DIST_NAME} &&  perl -p -i -e "s/^#plat-name.*/plat-name = ${FULL_PLAT_NAME}/" setup.cfg);
fi
(cd dist/${DIST_NAME} && ${PIP_BIN} wheel .)
(cp dist/${DIST_NAME}/*.whl dist)
