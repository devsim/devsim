#!/bin/bash
set -e

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
else
  echo "ERROR: FIRST ARGUMENT MUST BE gcc OR clang";
  exit 1;
fi


#minimal conda environments to prevent linking against the wrong libraries
if [ "${1}" = "gcc" ]
then
#conda create  -y --name python3_devsim_build python=3 mkl mkl-devel mkl-include boost cmake
# now opt for explicit dll load of mkl
conda create  -y --name python3_devsim_build python=3 boost cmake
fi

#This version does not use pardiso
if [ "${1}" = "clang" ]
then
conda create  -y --name python3_devsim_build python=3 boost cmake
fi
source activate python3_devsim_build

export PYTHON3_BIN=python
export PYTHON3_INCLUDE=$(python -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

#For macOS, the Xcode command line developer tools should be installed, these contain all the necessary libraries.  The math libraries are from the Apple Accelerate Framework.  Note that a FORTRAN compiler is not required.
#https://developer.apple.com/technologies/tools
#https://developer.apple.com/performance/accelerateframework.html

# SuperLU
#(cd external && curl -O http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz && tar xzf superlu_4.3.tar.gz)
(cd external && tar xzf superlu_4.3.tar.gz)

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
(cd external/SuperLU_4.3 && sh ../superlu_macos.sh)

# quad precision getrf
if [ "${1}" = "gcc" ]
then
(cd external/getrf && ./setup_osx.sh && cd build && make -j4)
fi


if [ "${1}" = "gcc" ]
then
bash ./scripts/setup_osx_gcc.sh
elif [ "${1}" = "clang" ]
then
bash ./scripts/setup_osx_10.10.sh
fi
(cd osx_x86_64_release && make -j4)
(cd dist && bash package_macos.sh ${1} devsim_macos_${2});


