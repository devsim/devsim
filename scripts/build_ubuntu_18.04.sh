#!/bin/bash
set -e
set -u
export DEVSIM_CONFIG="ubuntu_18.04"
export CMAKE="cmake"
export CMAKE_CXX_FLAGS=""
export CC="/usr/bin/gcc"
export CXX="/usr/bin/g++"
export F77="/usr/bin/gfortran"
export PYTHON3_BIN=python3
export PYTHON3_INCLUDE=$(python3 -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

# SYMDIFF build
(cd external/symdiff && bash ../symdiff_centos.sh && cd linux_x86_64_release && make -j2);

# SUPERLU build
(cd external/superlu && sh ../superlu_centos6.sh)

# quad precision getrf
(cd external/getrf && bash setup_centos6.sh && cd build && make -j2)

# start devsim build
bash scripts/setup_centos_6.sh
(cd linux_x86_64_release && make -j2)
(cd dist && bash package_linux.sh ${1})


