#!/bin/bash
set -e

export CMAKE="cmake"
export CMAKE_CXX_FLAGS=""
export CC="/usr/bin/gcc"
export CXX="/usr/bin/g++"
export F77="/usr/bin/gfortran"
export PYTHON3_BIN=python3
export PYTHON3_INCLUDE=$(python3 -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

#download boost library
#(cd ${HOME}/devsim/external && curl -O -L https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz)
#(cd ${HOME}/devsim/external && tar xzf boost_1_66_0.tar.gz)


# this script assumes git clone and submodule initialization has been done
#cd devsim

# SuperLU
#(cd external && curl -O http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz && tar xzf superlu_4.3.tar.gz)
(cd external && tar xzf superlu_4.3.tar.gz)

# SYMDIFF build
(cd external/symdiff && bash ../symdiff_ubuntu_18.04.sh && cd linux_x86_64_release && make -j2);

# CGNSLIB build
####(cd external && curl -L -O https://github.com/CGNS/CGNS/archive/v3.1.4.tar.gz && tar xzf v3.1.4.tar.gz)
####(cd external && mkdir -p CGNS-3.1.4/build && cd CGNS-3.1.4/build && cmake3  -DCMAKE_C_COMPILER=/opt/rh/devtoolset-8/root/usr/bin/gcc -DBUILD_CGNSTOOLS=OFF -DCMAKE_INSTALL_PREFIX=$PWD/../../cgnslib .. && make -j2 && make install)
# SUPERLU build
(cd external/SuperLU_4.3 && sh ../superlu_centos6.sh)

# quad precision getrf
(cd external/getrf && bash setup_centos6.sh && cd build && make -j2)


bash scripts/setup_ubuntu_18.04.sh
(cd linux_x86_64_release && make -j2)
(cd dist && bash package_linux.sh ${1})
