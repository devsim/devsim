#!/bin/bash
set -e
export PATH=/mingw64/bin:/usr/bin:${PATH}
# msys Specific
#pacman -Su --noconfirm rsync zip

GENERATOR="$1"
AOPTION="$2"
BUILDDIR="$3"
CONDA_PREFIX="$4"
PACKAGE_NAME="$5"
BASEDIR=$(pwd)

export PYTHON3_BIN=python
export PIP_BIN=pip
${PIP_BIN} install wheel
export PYTHON3_INCLUDE="${CONDA_PREFIX}/include"
export PYTHON3_ARCHIVE=$(cygpath -w ${CONDA_PREFIX}/libs/python3.lib)

cd "${BASEDIR}/external/umfpack_lgpl"
bash build_appveyor.sh "${GENERATOR}" "${AOPTION}" "${BUILDDIR}" "${CONDA_PREFIX}"

cd "${BASEDIR}/external/symdiff"
bash ../symdiff_appveyor.sh "${GENERATOR}" "${AOPTION}" "${BUILDDIR}" "${CONDA_PREFIX}"

cd "${BUILDDIR}"
cmake --build . --config Release -- //m //nologo //verbosity:minimal

cd "${BASEDIR}"
bash scripts/setup_appveyor.sh "${GENERATOR}" "${AOPTION}" "${BUILDDIR}" "${CONDA_PREFIX}"

cd "${BUILDDIR}"
cmake --build . --config Release -- //m //nologo //verbosity:minimal

cd "${BASEDIR}/dist"
bash package_appveyor.sh ${PACKAGE_NAME}

cd "${BASEDIR}/dist/${PACKAGE_NAME}"
cp -f ../bdist_wheel/setup.* .
${PIP_BIN} wheel .
cp *.whl ..
