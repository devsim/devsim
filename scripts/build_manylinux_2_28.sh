#!/bin/bash
set -e
set -u
export DEVSIM_ARCH=$(uname -m)
if [ ${DEVSIM_ARCH} = "x86_64" ]
then
export DEVSIM_CONFIG="centos_6"
elif [ ${DEVSIM_ARCH} = "aarch64" ]
then
export DEVSIM_CONFIG="nofloat128"
fi


yum install -y bison flex rsync

export CMAKE="cmake"
export CMAKE_CXX_FLAGS=""
export CC="gcc"
export CXX="g++"
export F77="gfortran"

export PYTHON3_BIN="python3.9"
export PIP_BIN="/opt/python/cp39-cp39/bin/pip"
${PIP_BIN} install wheel auditwheel
export PYTHON3_INCLUDE=$(${PYTHON3_BIN} -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

# SYMDIFF build
(cd external/symdiff && bash ../symdiff_centos.sh && cd linux_${DEVSIM_ARCH}_release && make -j4);

if [ ${DEVSIM_ARCH} = "x86_64" ]
then
# quad precision getrf
(cd external/getrf && bash setup_centos6.sh && cd build && make -j4)
fi

# umfpack support
(cd external/umfpack_lgpl && bash setup_centos6.sh && cd build && make -j4)

# start devsim build
if [ ${DEVSIM_ARCH} = "x86_64" ]
then
bash scripts/setup_centos_6.sh
elif [ ${DEVSIM_ARCH} = "aarch64" ]
then
bash scripts/setup_nofloat128.sh
fi

(cd linux_${DEVSIM_ARCH}_release && make -j4)
(cd dist && bash package_linux.sh ${1})
cp -f dist/bdist_wheel/setup.* dist/${1}/
(cd dist/${1} && ${PIP_BIN} wheel .)
${PIP_BIN} install auditwheel
(cd dist/${1} && ${PYTHON3_BIN} ../bdist_wheel/myauditwheel.py repair *.whl)
(cp dist/${1}/wheelhouse/*.whl dist)
