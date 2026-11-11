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
mkdir -p build/symdiff
${CMAKE} \
    -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -DCMAKE_CXX_COMPILER=${CXX} \
    -DTCLMAIN=OFF \
    -DPYTHON3=OFF \
    -B build/symdiff \
    -S external/symdiff

(cd build/symdiff && ${CMAKE} --build . -- -j2)

if [ ${DEVSIM_ARCH} = "x86_64" ]
then
# quad precision getrf
mkdir -p build/getrf
${CMAKE} \
    -DCMAKE_C_COMPILER=${CC} \
    -DCMAKE_CXX_COMPILER=${CXX} \
    -DCMAKE_Fortran_COMPILER=${F77} \
    -DCMAKE_Fortran_FLAGS="-freal-8-real-16 -ffixed-line-length-0 -fPIC" \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -B build/getrf \
    -S external/getrf
(cd build/getrf && ${CMAKE} --build . -- -j2)
fi

# umfpack support
mkdir -p build/umfpack_lgpl
${CMAKE} \
    -DCMAKE_C_COMPILER=${CC} \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -B build/umfpack_lgpl \
    -S external/umfpack_lgpl/umfpack
(cd build/umfpack_lgpl && ${CMAKE} --build . -- -j2)

# start devsim build
mkdir -p build/devsim
if [ ${DEVSIM_ARCH} = "x86_64" ]
then
${CMAKE} \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -DCMAKE_CXX_COMPILER=${CXX} \
    -DCMAKE_C_COMPILER=${CC} \
    -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
    -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
    -DDEVSIM_EXTENDED_PRECISION=ON \
    -DMKL_PARDISO=ON \
    -DPYTHON3=ON \
    -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
    -B build/devsim \
    -S .
(cd build/devsim && ${CMAKE} --build . -- -j2)
elif [ ${DEVSIM_ARCH} = "aarch64" ]
then
${CMAKE} \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -DCMAKE_CXX_COMPILER=${CXX} \
    -DCMAKE_C_COMPILER=${CC} \
    -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
    -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
    -DDEVSIM_EXTENDED_PRECISION=ON \
    -DDEVSIM_CPP_BIN_FLOAT=ON \
    -DEIGEN_DENSE_EXTENDED=ON \
    -DMKL_PARDISO=OFF \
    -DPYTHON3=ON \
    -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
    -B build/devsim \
    -S .
(cd build/devsim && ${CMAKE} --build . -- -j2)
fi

(cd dist && bash package_linux.sh ${1})
cp -f dist/bdist_wheel/setup.* dist/${1}/
(cd dist/${1} && ${PIP_BIN} wheel .)
${PIP_BIN} install auditwheel
(cd dist/${1} && ${PYTHON3_BIN} ../bdist_wheel/myauditwheel.py repair *.whl)
(cp dist/${1}/wheelhouse/*.whl dist)
