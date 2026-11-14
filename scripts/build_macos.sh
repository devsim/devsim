#!/bin/bash
set -e
set -x
set -u
if [ "$#" -ne 1 ]; then
    echo "usage: build_macos.sh version" >&2
    exit 2
fi
case "$1" in
    ""|*[!a-zA-Z0-9._-]*)
        echo "version must contain only letters, numbers, dots, underscores, and hyphens" >&2
        exit 2
        ;;
esac
DEVSIM_CONFIG="osx_10.10"

export CMAKE="cmake"
export CMAKE_CXX_FLAGS=""
export CC=clang;
export CXX=clang++;
export F77="";
export ARCH_ARG="-DCMAKE_OSX_ARCHITECTURES=arm64"
export PLAT_NAME="arm64"
#FULL_PLAT_NAME=${PLAT_NAME}
export PYTHON3_BIN="${PYTHON3_BIN:-/opt/homebrew/bin/python3}"

"${PYTHON3_BIN}" -c 'import sys, sysconfig; sys.exit("A regular CPython 3.15 or newer is required") if sys.implementation.name != "cpython" or sys.version_info < (3, 15) or sysconfig.get_config_var("Py_GIL_DISABLED") else None'
export PYTHON3_INCLUDE=$(${PYTHON3_BIN} -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

DIST_NAME=devsim_macos_${PLAT_NAME}_${1}
DIST_ROOT=$(cd dist && pwd -P)
for ABI in abi3 abi3t; do
    STAGING_DIR=${DIST_ROOT}/${DIST_NAME}_${ABI}
    if [ -L "${STAGING_DIR}" ] || { [ -e "${STAGING_DIR}" ] && [ ! -d "${STAGING_DIR}" ]; }; then
        echo "Staging path is not a directory: ${STAGING_DIR}" >&2
        exit 1
    fi
done

#For macOS, the Xcode command line developer tools should be installed, these contain all the necessary libraries.  The math libraries are from the Apple Accelerate Framework.  Note that a FORTRAN compiler is not required.
#https://developer.apple.com/technologies/tools
#https://developer.apple.com/performance/accelerateframework.html

# SYMDIFF build
mkdir -p build/symdiff
${CMAKE} \
    ${ARCH_ARG} \
    -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -DCMAKE_CXX_COMPILER=${CXX} \
    -DTCLMAIN=OFF \
    -DPYTHON3=OFF \
    -B build/symdiff \
    -S external/symdiff

(cd build/symdiff && ${CMAKE} --build . -- -j2)

# umfpack support
mkdir -p build/umfpack_lgpl
${CMAKE} \
    ${ARCH_ARG} \
    -DCMAKE_C_COMPILER=${CC} \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -B build/umfpack_lgpl \
    -S external/umfpack_lgpl/umfpack
(cd build/umfpack_lgpl && ${CMAKE} --build . -- -j2)

mkdir -p build/devsim
${CMAKE} \
    ${ARCH_ARG} \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -DCMAKE_CXX_COMPILER=${CXX} \
    -DCMAKE_C_COMPILER=${CC} \
    -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
    -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
    -DDEVSIM_EXTENDED_PRECISION=ON \
    -DDEVSIM_CPP_BIN_FLOAT=ON \
    -DEIGEN_DENSE_EXTENDED=ON \
    -DMKL_PARDISO=ON \
    -DPYTHON3=ON \
    -DPYTHON3T=ON \
    -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
    -DPYTHON3T_INCLUDE=${PYTHON3_INCLUDE} \
    -B build/devsim \
    -S .
(cd build/devsim && ${CMAKE} --build . -- -j2)

for ABI in abi3 abi3t; do
    STAGING_DIR=${DIST_ROOT}/${DIST_NAME}_${ABI}
    if [ -L "${STAGING_DIR}" ] || { [ -e "${STAGING_DIR}" ] && [ ! -d "${STAGING_DIR}" ]; }; then
        echo "Staging path is not a directory: ${STAGING_DIR}" >&2
        exit 1
    fi
    if [ -d "${STAGING_DIR}" ]; then
        rm -rf -- "${STAGING_DIR}"
    fi
    (cd dist && bash package_macos.sh "${DIST_NAME}_${ABI}" "${ABI}")
    cp -f dist/bdist_wheel/pyproject.toml dist/bdist_wheel/setup.py "${STAGING_DIR}"
    DEVSIM_WHEEL_ABI="${ABI}" "${PYTHON3_BIN}" -m pip wheel --no-deps \
        --wheel-dir dist "${STAGING_DIR}"
done
