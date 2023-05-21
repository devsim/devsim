#!/bin/bash
set -e
set -u
export DEVSIM_CONFIG="centos_6"


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
(cd external/symdiff && bash ../symdiff_centos.sh && cd linux_x86_64_release && make -j3);

# quad precision getrf
(cd external/getrf && bash setup_centos6.sh && cd build && make -j3)

# umfpack support
(cd external/umfpack_lgpl && bash setup_centos6.sh && cd build && make -j3)

# start devsim build
bash scripts/setup_centos_6.sh

## overcome bad libstdc++ in anaconda directory
#mkdir -p linux_x86_64_release/linklibs
#for i in libz.a libsqlite3*; do cp -f ${CONDA_PREFIX}/lib/$i linux_x86_64_release/linklibs/; done

(cd linux_x86_64_release && make -j3)
(cd dist && bash package_linux.sh ${1})
cp -f dist/bdist_wheel/setup.* dist/${1}/
(cd dist/${1} && ${PIP_BIN} wheel .)
${PIP_BIN} install auditwheel
(cd dist/${1} && ${PYTHON3_BIN} ../bdist_wheel/myauditwheel.py repair *.whl)
(cp dist/${1}/wheelhouse/*.whl dist)
