#!/bin/bash
set -e

# Centos Specific
#https://fedoraproject.org/wiki/EPEL
yum install -y git bison flex zlib-static zlib-devel sqlite-devel python3 python3-devel
yum install -y centos-release-scl
yum install -y devtoolset-9-gcc devtoolset-9-gcc-c++ devtoolset-9-libquadmath-devel devtoolset-9-gcc-gfortran make
yum install -y epel-release
yum install -y cmake3

export CMAKE="cmake3"
export CMAKE_CXX_FLAGS=""
export CC="/opt/rh/devtoolset-9/root/usr/bin/gcc"
export CXX="/opt/rh/devtoolset-9/root/usr/bin/g++"
export F77="/opt/rh/devtoolset-9/root/usr/bin/gfortran"

#minimal conda environments to prevent linking against the wrong libraries
# now opt for explicit dll load of mkl
#conda create  -y --name python3_devsim_build python=3 mkl mkl-devel mkl-include boost cmake
#conda create  -y --name python3_devsim_build python=3 boost cmake nomkl
#source activate python3_devsim_build

export PYTHON3_BIN=python3
export PYTHON3_INCLUDE=$(${PYTHON3_BIN} -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

# SYMDIFF build
(cd external/symdiff && bash ../symdiff_centos.sh && cd linux_x86_64_release && make -j2);

# SUPERLU build
(cd external/superlu && sh ../superlu_centos6.sh)

# quad precision getrf
(cd external/getrf && bash setup_centos6.sh && cd build && make -j2)

#
(cd external && curl -L -O https://boostorg.jfrog.io/artifactory/main/release/1.80.0/source/boost_1_80_0.tar.gz && tar xzf boost_1_80_0.tar.gz)

# start devsim build
bash scripts/setup_centos_6.sh

## overcome bad libstdc++ in anaconda directory
#mkdir -p linux_x86_64_release/linklibs
#for i in libz.a libsqlite3*; do cp -f ${CONDA_PREFIX}/lib/$i linux_x86_64_release/linklibs/; done

(cd linux_x86_64_release && make -j2)
(cd dist && bash package_linux.sh ${1})


