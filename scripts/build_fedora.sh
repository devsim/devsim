#!/bin/bash
set -e

DSDIR=$PWD

# Fedora Specific
#https://fedoraproject.org/wiki/EPEL
# in Docker, the default user is root, and sudo is not installed
SUDO=""
if [[ ${EUID} -ne 0 ]]; then
  SUDO=sudo
fi
${SUDO} dnf install -y git bison flex tcl tcl-devel cmake3 gcc gcc-c++ libquadmath-devel \
     gcc-gfortran bzip2 cgnslib cgnslib-devel \
     sqlite-devel python3-devel blas-devel lapack-devel \
     make rsync

export CMAKE="cmake"
export CMAKE_CXX_FLAGS=""
export CC=gcc
export CXX=g++
export F77=gfortran
export PYTHON3_BIN=python3
export PYTHON3_INCLUDE=$(python3 -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

git submodule init &&
git submodule update

# quad precision getrf
(
cd ${DSDIR}/external/getrf &&
mkdir -p build &&
cd build &&
cmake -DCMAKE_Fortran_FLAGS="-freal-8-real-16 -ffixed-line-length-0 -fPIC" -DCMAKE_BUILD_TYPE=RELEASE .. &&
make -j4
)

# SYMDIFF build
(
cd ${DSDIR}/external/symdiff &&
${DSDIR}/scripts/setup_fedora.sh &&
cd linux_x86_64_release && make -i -j4
)

# Finally, build devsim
(
cd ${DSDIR} &&
scripts/setup_fedora.sh &&
cd linux_x86_64_release && make -j4
)
