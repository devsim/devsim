#!/bin/bash
set -e
set -x
set -u
export PATH=/mingw64/bin:/usr/bin:${PATH}

GENERATOR="$1"
AOPTION="$2"
TOOLSET="$3"
BUILDDIR="$4"
PYTHON3_BIN="$5"
PACKAGE_NAME="$6"
BASEDIR=$(pwd)

export PYTHON3_BIN="${PYTHON3_BIN:-python}"
case "${PYTHON3_BIN}" in
    [A-Za-z]:\\*) export PYTHON3_BIN=$(cygpath -u "${PYTHON3_BIN}");;
esac
export PIP_BIN=pip
${PIP_BIN} install wheel
export PYTHON3_INCLUDE=$(${PYTHON3_BIN} -c "import sysconfig; print(sysconfig.get_path('include'))")
export PYTHON3T_INCLUDE=${PYTHON3_INCLUDE}
export PYTHON3_ARCHIVE=$(${PYTHON3_BIN} -c "import sysconfig; import os; print(os.path.join(sysconfig.get_config_var('LIBDIR'),'python3.lib'))")
export PYTHON3T_ARCHIVE=$(${PYTHON3_BIN} -c "import sysconfig; import os; print(os.path.join(sysconfig.get_config_var('LIBDIR'),'python3t.lib'))")
export DEVSIM_CONFIG=appveyor


# SYMDIFF build
mkdir -p ${BUILDDIR}/symdiff
cmake -G "${GENERATOR}" \
      -A "${AOPTION}" \
      -T "${TOOLSET}" \
      -DTCLMAIN=OFF \
      -DPYTHON3=OFF \
      -B "${BUILDDIR}/symdiff" \
      -S "${BASEDIR}/external/symdiff"

cmake --build ${BUILDDIR}/symdiff --config Release --parallel 4 -- //nologo //verbosity:minimal

# umfpack support
mkdir -p ${BUILDDIR}/umfpack_lgpl
cmake -G "${GENERATOR}" \
      -A "${AOPTION}" \
      -T "${TOOLSET}" \
      -B "${BUILDDIR}/umfpack_lgpl" \
      -S "${BASEDIR}/external/umfpack_lgpl/umfpack"
cmake --build ${BUILDDIR}/umfpack_lgpl --config Release --parallel 4 -- //nologo //verbosity:minimal \

mkdir -p ${BUILDDIR}/devsim
cmake -G "${GENERATOR}" \
    -A ${AOPTION} \
    -T ${TOOLSET} \
    -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
    -DDEVSIM_EXTENDED_PRECISION=ON \
    -DDEVSIM_CPP_BIN_FLOAT=ON \
    -DEIGEN_DENSE_EXTENDED=ON \
    -DMKL_PARDISO=ON \
    -DPYTHON3=ON \
    -DPYTHON3T=ON \
    -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
    -DPYTHON3T_INCLUDE=${PYTHON3T_INCLUDE} \
    -DPYTHON3_ARCHIVE=${PYTHON3_ARCHIVE} \
    -DPYTHON3T_ARCHIVE=${PYTHON3T_ARCHIVE} \
    -B ${BUILDDIR}/devsim \
    -S ${BASEDIR}
cmake --build ${BUILDDIR}/devsim --config Release --parallel 4 -- //nologo //verbosity:minimal

cd "${BASEDIR}/dist"
bash package_appveyor.sh ${PACKAGE_NAME}

cd "${BASEDIR}"
DIST_NAME=${PACKAGE_NAME}
DIST_ROOT=$(cd dist && pwd -P)
for ABI in abi3 abi3t; do
    STAGING_DIR=${DIST_ROOT}/${DIST_NAME}_${ABI}
    if [ -L "${STAGING_DIR}" ] || { [ -e "${STAGING_DIR}" ] && [ ! -d "${STAGING_DIR}" ]; }; then
        echo "Staging path is not a directory: ${STAGING_DIR}" >&2
        exit 1
    fi
    if [ -d "${STAGING_DIR}" ]; then
        rm -rf -- "${STAGING_DIR}"
    fi
    (cd dist && bash package_appveyor.sh "${DIST_NAME}_${ABI}" "${ABI}")
    cp -f dist/bdist_wheel/pyproject.toml dist/bdist_wheel/setup.py "${STAGING_DIR}"
    DEVSIM_WHEEL_ABI="${ABI}" "${PYTHON3_BIN}" -m pip wheel --no-deps \
        --wheel-dir dist "${STAGING_DIR}"
done
