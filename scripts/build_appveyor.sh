#!/bin/bash
set -e
set -x
export PATH=/mingw64/bin:/usr/bin:${PATH}
# msys Specific
#pacman -Su --noconfirm rsync zip

GENERATOR="$1"
AOPTION="$2"
TOOLSET="$3"
BUILDDIR="$4"
CONDA_PREFIX="$5"
PACKAGE_NAME="$6"
BASEDIR=$(pwd)

export PYTHON3_BIN=python
export PIP_BIN=pip
${PIP_BIN} install wheel
export PYTHON3_INCLUDE=$(${PYTHON3_BIN} -c "import sysconfig; print(sysconfig.get_path('include'))")
export PYTHON3_ARCHIVE=$(cygpath -w ${CONDA_PREFIX}/libs/python3.lib)

cd "${BASEDIR}/external/umfpack_lgpl"
bash build_appveyor.sh "${GENERATOR}" "${AOPTION}" "${TOOLSET}" "${BUILDDIR}" "${CONDA_PREFIX}"

cd "${BASEDIR}/external/symdiff"
mkdir -p ${BUILDDIR}
(cd ${BUILDDIR}; cmake -G "${GENERATOR}" -A "${AOPTION}" -T "${TOOLSET}" -DSYMDIFF_CONFIG="${SYMDIFF_CONFIG}" -DANACONDA_PATH="${CONDA_PREFIX}" -DTCLMAIN=OFF -DPYTHON3=OFF ..)

cd "${BUILDDIR}"
cmake --build . --config Release --parallel 4 -- //nologo //verbosity:minimal

cd "${BASEDIR}"
bash scripts/setup_appveyor.sh "${GENERATOR}" "${AOPTION}" "${TOOLSET}" "${BUILDDIR}" "${CONDA_PREFIX}"

cd "${BUILDDIR}"
cmake --build . --config Release --parallel 4 -- //nologo //verbosity:minimal

cd "${BASEDIR}/dist"
bash package_appveyor.sh ${PACKAGE_NAME}

cd "${BASEDIR}/dist/${PACKAGE_NAME}"
#TODO: debug
echo "$PWD"
cp -f ../bdist_wheel/setup.* .
${PIP_BIN} wheel .
cp *.whl ..
