#!/bin/bash
set -e

if [ "$1" = "gcc" ]
  then
  brew update > /dev/null;
  if brew ls --versions gcc > /dev/null;
  then
  brew outdated gcc || brew upgrade gcc;
  else
  brew install gcc;
  fi
  export CC=/usr/local/bin/gcc-7;
  export CXX=/usr/local/bin/g++-7;
  export F77=/usr/local/bin/gfortran-7;
elif [ "$1" = "clang" ]
  then
  export CC=/usr/bin/gcc;
  export CXX=/usr/bin/g++;
  export F77="";
else
  echo "ERROR: FIRST ARGUMENT MUST BE gcc OR clang";
  exit 1;
fi

#For Mac OS X, the Xcode command line developer tools should be installed, these contain all the necessary libraries.  The math libraries are from the Apple Accelerate Framework.  Note that a FORTRAN compiler is not required.
#https://developer.apple.com/technologies/tools
#https://developer.apple.com/performance/accelerateframework.html
#In addition, cmake is needed for Mac OS X.  The package may be downloaded from:
#http://www.cmake.org

# put the tag name in first argument used for distribution
# this script assumes git clone and submodule initialization has been done

# SuperLU and CGNS Download
if [ ! -f external/superlu_4.3.tar.gz ]
then
(cd external && curl -O http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz && tar xzf superlu_4.3.tar.gz)
fi

if [ ! -f external/cgnslib_3.1.4-2.tar.gz ]
then
(cd external && curl -L -O http://downloads.sourceforge.net/project/cgns/cgnslib_3.1/cgnslib_3.1.4-2.tar.gz && tar xzf cgnslib_3.1.4-2.tar.gz)
fi

# SYMDIFF build
if [ "$1" = "gcc" ]
then
(cd external/symdiff && bash scripts/setup_osx_gcc.sh && cd osx_release && make -j2)
elif [ "$1" = "clang" ]
then
(cd external/symdiff && bash scripts/setup_osx_10.10.sh && cd osx_release && make -j2)
fi

# CGNSLIB build
(cd external && mkdir -p cgnslib_3.1.4/build && cd cgnslib_3.1.4/build && cmake -DCMAKE_C_COMPILER=${CC} -DBUILD_CGNSTOOLS=OFF -DCMAKE_INSTALL_PREFIX=$PWD/../../cgnslib .. && make -j2 && make install)
# SUPERLU build
(cd external/SuperLU_4.3 && sh ../superlu_osx_10.10.sh)

if [ "$1" = "gcc" ]
then
(cd external/getrf && ./setup_osx.sh && cd build && make -j2)
fi


if [ "$1" = "gcc" ]
then
bash ./scripts/setup_osx_gcc.sh
elif [ "$1" = "clang" ]
then
bash ./scripts/setup_osx_10.10.sh
fi
(cd osx_x86_64_release && make -j2)
if [ ! -z "${2}" ]
then
(cd dist && bash package_apple.sh devsim_osx_${2});
fi


