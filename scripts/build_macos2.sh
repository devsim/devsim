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
  export CC=/usr/local/bin/gcc-12;
  export CXX=/usr/local/bin/g++-12;
  export F77=/usr/local/bin/gfortran-12;
  export ARCH_ARG=""
  export PLAT_NAME="x86_64"
#  brew unlink gcc && brew link gcc

  # https://github.com/Microsoft/LightGBM/pull/1560
  # removes symlink
  #rm -f '/usr/local/include/c++'
  # fix "fatal error: _stdio.h: No such file or directory"
# try moving to osx_image: xcode10.1
#  sudo softwareupdate -i "Command Line Tools (macOS High Sierra version 10.13) for Xcode-9.3"

elif [ "${1}" = "clang" ]
  then
  export CMAKE="cmake"
  export CMAKE_CXX_FLAGS=""
  export CC=/usr/bin/gcc;
  export CXX=/usr/bin/g++;
  export F77="";
  export ARCH_ARG='-DCMAKE_OSX_ARCHITECTURES="arm64"'
  export PLAT_NAME="arm64"
else
  echo "ERROR: FIRST ARGUMENT MUST BE gcc OR clang";
  exit 1;
fi


##minimal conda environments to prevent linking against the wrong libraries
#if [ "${1}" = "gcc" ]
#then
## now opt for explicit dll load of mkl
#conda create  -y --name python3_devsim_build python=3 cmake nomkl
#fi
#
##This version does not use pardiso
#if [ "${1}" = "clang" ]
#then
#conda create  -y --name python3_devsim_build python=3 cmake nomkl
#fi
#source activate python3_devsim_build

export PYTHON3_BIN=python3
export PIP_BIN=pip3
${PIP_BIN} install wheel
export PYTHON3_INCLUDE=$(${PYTHON3_BIN} -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

#For macOS, the Xcode command line developer tools should be installed, these contain all the necessary libraries.  The math libraries are from the Apple Accelerate Framework.  Note that a FORTRAN compiler is not required.
#https://developer.apple.com/technologies/tools
#https://developer.apple.com/performance/accelerateframework.html

# SuperLU
#(cd external && curl -O http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz && tar xzf superlu_4.3.tar.gz)
#(cd external && tar xzf superlu_4.3.tar.gz)

# SYMDIFF build
if [ "${1}" = "gcc" ]
then
(cd external/symdiff && bash ../symdiff_macos.sh && cd osx_release && make -j4)
elif [ "${1}" = "clang" ]
then
(cd external/symdiff && bash  ../symdiff_macos.sh && cd osx_release && make -j4)
fi

# CGNSLIB build
####if [ ! -f external/v3.1.4.tar.gz ]
####then
####(cd external && curl -L -O https://github.com/CGNS/CGNS/archive/v3.1.4.tar.gz && tar xzf v3.1.4.tar.gz)
####fi
####(cd external && mkdir -p CGNS-3.1.4/build && cd CGNS-3.1.4/build && cmake -DCMAKE_C_COMPILER=${CC} -DBUILD_CGNSTOOLS=OFF -DCMAKE_INSTALL_PREFIX=$PWD/../../cgnslib .. && make -j4 && make install)

# SUPERLU build
(cd external/superlu && bash ../superlu_macos.sh)

# quad precision getrf
if [ "${1}" = "gcc" ]
then
(cd external/getrf && ./setup_osx.sh && cd build && make -j4)
fi

# umfpack support
(cd external/umfpack_lgpl && bash setup_macos.sh && cd build && make -j2)

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
(cd dist/${DIST_NAME} &&  perl -p -i -e "s/^#plat-name.*/plat-name = ${FULL_PLAT_NAME}/" setup.cfg)
(cd dist/${DIST_NAME} && ${PIP_BIN} wheel .)
(cp dist/${DIST_NAME}/*.whl dist)
