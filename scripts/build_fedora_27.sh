#!/bin/bash
set -e
# put the tag name in first argument used for distribution
# this script assumes git clone and submodule initialization has been done

DSDIR=${HOME}/DevSim

# Fedora Specific
#https://fedoraproject.org/wiki/EPEL
# in Docker, the default user is root, and sudo is not installed
SUDO=""
if [[ ${EUID} -ne 0 ]]; then
  SUDO=sudo
fi
${SUDO} dnf install -y git bison flex tcl tcl-devel cmake3 gcc gcc-c++ libquadmath-devel \
     gcc-gfortran bzip2 boost boost-devel cgnslib cgnslib-devel \
     sqlite-devel python2-devel python3-devel blas-devel lapack-devel \
     make

mkdir -p ${DSDIR}

# add max depth or copy over
if [ ! -d ${DSDIR}/devsim ]; then
    (
    cd ${DSDIR} &&
    git clone https://github.com/devsim/devsim &&
    cd devsim &&
    git submodule init &&
    git submodule update
    )
fi

# SuperLU build
(
cd ${DSDIR}/devsim/external &&
if [ ! -d SuperLU_4.3 ]; then
    curl -O http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz &&
    tar xzf superlu_4.3.tar.gz &&
    cd SuperLU_4.3 &&
    make CC=/usr/bin/gcc SuperLUroot=`pwd` SUPERLULIB=`pwd`/lib/libsuperlu_4.3.a NOOPTS="-fPIC" CFLAGS="-O3 -DPRNTlevel=0 -fPIC" CDEFS="-Dadd_" superlulib
fi
)

# quad precision getrf
(
cd ${DSDIR}/devsim/external/getrf &&
mkdir -p build &&
cd build &&
cmake -DCMAKE_Fortran_FLAGS="-freal-8-real-16 -ffixed-line-length-0" -DCMAKE_BUILD_TYPE=RELEASE .. &&
make -j4
)



# SYMDIFF build
(
cd ${DSDIR}/devsim/external/symdiff &&
./scripts/setup_fedora_27.sh &&
cd linux_x86_64_release && make -j4
)

# Finally, build devsim
(
cd ${DSDIR}/devsim &&
scripts/setup_fedora_27.sh &&
cd linux_x86_64_release && make -j4
)
#(cd dist && bash package_linux.sh ${1})


