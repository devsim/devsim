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

cd "${BASEDIR}/external"
bash ./build_superlu_appveyor.sh "${GENERATOR}" "${AOPTION}" "${BUILDDIR}"

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

