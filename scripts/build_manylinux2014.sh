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


# Centos Specific
#https://fedoraproject.org/wiki/EPEL
#yum install -y git bison flex zlib-static zlib-devel sqlite-devel python3 python3-devel
#yum install -y centos-release-scl
#yum install -y devtoolset-9-gcc devtoolset-9-gcc-c++ devtoolset-9-libquadmath-devel devtoolset-9-gcc-gfortran make
#yum install -y epel-release
#yum install -y cmake3
yum install -y bison flex rsync

export CMAKE="cmake"
export CMAKE_CXX_FLAGS=""
export CC="gcc"
export CXX="g++"
export F77="gfortran"

export PYTHON3_BIN=python3.7
export PIP_BIN=/opt/python/cp37-cp37m/bin/pip
${PIP_BIN} install wheel auditwheel
export PYTHON3_INCLUDE=$(${PYTHON3_BIN} -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

# SYMDIFF build
(cd external/symdiff && bash ../symdiff_centos.sh && cd linux_${DEVSIM_ARCH}_release && make -j3);

if [ ${DEVSIM_ARCH} = "x86_64" ]
then
# quad precision getrf
(cd external/getrf && bash setup_centos6.sh && cd build && make -j3)
fi

# umfpack support
(cd external/umfpack_lgpl && bash setup_centos6.sh && cd build && make -j3)

# start devsim build
if [ ${DEVSIM_ARCH} = "x86_64" ]
then
bash scripts/setup_centos_6.sh
elif [ ${DEVSIM_ARCH} = "aarch64" ]
then
bash scripts/setup_nofloat128.sh
fi

## overcome bad libstdc++ in anaconda directory
#mkdir -p linux_${DEVSIM_ARCH}_release/linklibs
#for i in libz.a libsqlite3*; do cp -f ${CONDA_PREFIX}/lib/$i linux_${DEVSIM_ARCH}_release/linklibs/; done

(cd linux_${DEVSIM_ARCH}_release && make -j3)
(cd dist && bash package_linux.sh ${1})
cp -f dist/bdist_wheel/setup.* dist/${1}/
(cd dist/${1} && ${PIP_BIN} wheel .)
${PIP_BIN} install auditwheel
(cd dist/${1} && ${PYTHON3_BIN} ../bdist_wheel/myauditwheel.py repair *.whl)
(cp dist/${1}/wheelhouse/*.whl dist)
