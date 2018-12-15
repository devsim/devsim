#!/bin/bash
set -e

if [ "${1}" = "gcc" ]
  then
  brew update > /dev/null;
  #if brew ls --versions gcc > /dev/null;
  #then
  #brew outdated gcc || brew upgrade gcc;
  #else
  ## the overwrite is to fix the linking issue seen on travis ci
  #brew install gcc || brew link --overwrite gcc;
  #fi
  #export CC=/usr/local/bin/gcc-8;
  #export CXX=/usr/local/bin/g++-8;
  #export F77=/usr/local/bin/gfortran-8;
  brew install gcc@8
  brew link gcc
  export CC=gcc-8;
  export CXX=g++-8;
  export F77=gfortran-8;

  # https://github.com/Microsoft/LightGBM/pull/1560
  # removes symlink
  #rm -f '/usr/local/include/c++'
  # fix "fatal error: _stdio.h: No such file or directory"
# try moving to osx_image: xcode10.1
#  sudo softwareupdate -i "Command Line Tools (macOS High Sierra version 10.13) for Xcode-9.3"

  # install boost
  if brew ls --versions boost > /dev/null;
  then
  brew outdated boost || brew upgrade boost;
  else
  brew install boost
  fi

elif [ "${1}" = "clang" ]
  then
  export CC=/usr/bin/gcc;
  export CXX=/usr/bin/g++;
  export F77="";
else
  echo "ERROR: FIRST ARGUMENT MUST BE gcc OR clang";
  exit 1;
fi

#minimal conda environments to prevent linking against the wrong libraries
if [ "${1}" = "gcc" ] && [ ! -f ${HOME}/Miniconda2-latest-MacOSX-x86_64.sh ]
then
(cd ${HOME} &&
curl -O https://repo.continuum.io/miniconda/Miniconda2-latest-MacOSX-x86_64.sh;
bash ~/Miniconda2-latest-MacOSX-x86_64.sh -b -p ${HOME}/anaconda;)
# Python 2
${HOME}/anaconda/bin/conda create  -y --name python27_devsim_build python=2.7
${HOME}/anaconda/bin/conda install -y --name python27_devsim_build mkl mkl-devel mkl-include
#Python3
${HOME}/anaconda/bin/conda create -y --name python36_devsim_build python=3.6
${HOME}/anaconda/bin/conda create -y --name python37_devsim_build python=3.7
fi


#For macOS, the Xcode command line developer tools should be installed, these contain all the necessary libraries.  The math libraries are from the Apple Accelerate Framework.  Note that a FORTRAN compiler is not required.
#https://developer.apple.com/technologies/tools
#https://developer.apple.com/performance/accelerateframework.html
#In addition, cmake is needed for macOS.  The package may be downloaded from:
#http://www.cmake.org

# put the tag name in first argument used for distribution
# this script assumes git clone and submodule initialization has been done

# SuperLU
if [ ! -f external/superlu_4.3.tar.gz ]
then
(cd external && curl -O http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz && tar xzf superlu_4.3.tar.gz)
fi

# SYMDIFF build
if [ "${1}" = "gcc" ]
then
(cd external/symdiff && bash scripts/setup_osx_gcc.sh && cd osx_release && make -j4)
elif [ "${1}" = "clang" ]
then
(cd external/symdiff && bash scripts/setup_osx_10.10.sh && cd osx_release && make -j4)
fi

# CGNSLIB build
####if [ ! -f external/v3.1.4.tar.gz ]
####then
####(cd external && curl -L -O https://github.com/CGNS/CGNS/archive/v3.1.4.tar.gz && tar xzf v3.1.4.tar.gz)
####fi
####(cd external && mkdir -p CGNS-3.1.4/build && cd CGNS-3.1.4/build && cmake -DCMAKE_C_COMPILER=${CC} -DBUILD_CGNSTOOLS=OFF -DCMAKE_INSTALL_PREFIX=$PWD/../../cgnslib .. && make -j4 && make install)

# SUPERLU build
(cd external/SuperLU_4.3 && sh ../superlu_osx_10.10.sh)

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
if [ ! -z "${2}" ]
then
(cd dist && bash package_macos.sh ${1} devsim_macos_${2});
fi


