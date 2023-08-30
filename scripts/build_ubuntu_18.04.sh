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
export PIP_BIN=pip
${PIP_BIN} install wheel auditwheel
export PYTHON3_INCLUDE=$(python3 -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

# SYMDIFF build
(cd external/symdiff && bash ../symdiff_centos.sh && cd linux_x86_64_release && make -j3);

# quad precision getrf
(cd external/getrf && bash setup_centos6.sh && cd build && make -j3)

# umfpack support
(cd external/umfpack_lgpl && bash setup_centos6.sh && cd build && make -j3)

# start devsim build
bash scripts/setup_centos_6.sh
(cd linux_x86_64_release && make -j3)
(cd dist && bash package_linux.sh ${1})
cp -f dist/bdist_wheel/setup.* dist/${1}/
(cd dist/${1} && ${PIP_BIN} wheel .)
#${PIP_BIN} install auditwheel
#(cd dist/${1} && ${PYTHON3_BIN} ../bdist_wheel/myauditwheel.py repair *.whl)
#(cp dist/${1}/wheelhouse/*.whl dist)
