#!/bin/bash
set -e

#For Mac OS X, the Xcode command line developer tools should be installed, these contain all the necessary libraries.  The math libraries are from the Apple Accelerate Framework.  Note that a FORTRAN compiler is not required.
#https://developer.apple.com/technologies/tools
#https://developer.apple.com/performance/accelerateframework.html
#In addition, cmake is needed for Mac OS X.  The package may be downloaded from:
#http://www.cmake.org

# put the tag name in first argument used for distribution
# this script assumes git clone and submodule initialization has been done

# SuperLU and CGNS Download
(cd external && curl -O http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz && tar xzf superlu_4.3.tar.gz)
(cd external && curl -L -O http://downloads.sourceforge.net/project/cgns/cgnslib_3.1/cgnslib_3.1.4-2.tar.gz && tar xzf cgnslib_3.1.4-2.tar.gz)

# SYMDIFF build
(cd external/symdiff && bash scripts/setup_osx_10.10.sh && cd osx_release && make -j2)
# CGNSLIB build
(cd external && mkdir -p cgnslib_3.1.4/build && cd cgnslib_3.1.4/build && cmake -DBUILD_CGNSTOOLS=OFF -DCMAKE_INSTALL_PREFIX=$PWD/../../cgnslib .. && make -j2 && make install)
# SUPERLU build
(cd external/SuperLU_4.3 && sh ../superlu_osx_10.10.sh)


bash ./scripts/setup_osx_10.10.sh
(cd osx_x86_64_release && make -j2)
(cd dist && bash package_apple.sh devsim_osx_${TRAVIS_TAG});


