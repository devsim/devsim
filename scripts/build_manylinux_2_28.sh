#!/bin/bash
set -e
set -x
set -u
if [ "$#" -ne 1 ]; then
    echo "usage: build_manylinux_2_28.sh version" >&2
    exit 2
fi
case "$1" in
    ""|*[!a-zA-Z0-9._-]*)
        echo "version must contain only letters, numbers, dots, underscores, and hyphens" >&2
        exit 2
        ;;
esac
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

export PYTHON3_BIN="${PYTHON3_BIN:-/opt/python/cp315-cp315/bin/python}"
"${PYTHON3_BIN}" -c 'import sys, sysconfig; sys.exit("A regular CPython 3.15 or newer is required") if sys.implementation.name != "cpython" or sys.version_info < (3, 15) or sysconfig.get_config_var("Py_GIL_DISABLED") else None'
"${PYTHON3_BIN}" -m pip install wheel auditwheel
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
    -DPYTHON3T=ON \
    -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
    -DPYTHON3T_INCLUDE=${PYTHON3_INCLUDE} \
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
    -DPYTHON3T=ON \
    -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
    -DPYTHON3T_INCLUDE=${PYTHON3_INCLUDE} \
    -B build/devsim \
    -S .
(cd build/devsim && ${CMAKE} --build . -- -j2)
fi

DIST_ROOT=$(cd dist && pwd -P)
for ABI in abi3 abi3t; do
    DIST_NAME=${1}_${ABI}
    STAGING_DIR=${DIST_ROOT}/${DIST_NAME}
    if [ -L "${STAGING_DIR}" ] || { [ -e "${STAGING_DIR}" ] && [ ! -d "${STAGING_DIR}" ]; }; then
        echo "Staging path is not a directory: ${STAGING_DIR}" >&2
        exit 1
    fi
    if [ -d "${STAGING_DIR}" ]; then
        rm -rf -- "${STAGING_DIR}"
    fi
    (cd dist && bash package_linux.sh "${DIST_NAME}" "${ABI}")
    cp -f dist/bdist_wheel/pyproject.toml dist/bdist_wheel/setup.py "${STAGING_DIR}/"
    (cd "${STAGING_DIR}" && DEVSIM_WHEEL_ABI="${ABI}" "${PYTHON3_BIN}" -m pip wheel --no-deps .)
    (cd "${STAGING_DIR}" && "${PYTHON3_BIN}" ../bdist_wheel/myauditwheel.py repair *.whl)
    cp "${STAGING_DIR}"/wheelhouse/*.whl dist
done
