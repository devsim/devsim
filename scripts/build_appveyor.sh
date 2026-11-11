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
export DEVSIM_CONFIG=appveyor

mkdir -p ${BUILDDIR}/umfpack_lgpl
cmake -G "${GENERATOR}" \
      -A "${AOPTION}" \
      -T "${TOOLSET}" \
      -B "${BUILDDIR}/umfpack_lgpl" \
      -S "${BASEDIR}/external/umfpack_lgpl/umfpack"
cmake --build ${BUILDDIR}/umfpack_lgpl --config Release --parallel 4 -- //nologo //verbosity:minimal \

mkdir -p ${BUILDDIR}/symdiff
cmake -G "${GENERATOR}" \
      -A "${AOPTION}" \
      -T "${TOOLSET}" \
      -DSYMDIFF_CONFIG="${SYMDIFF_CONFIG}" \
      -DANACONDA_PATH="${CONDA_PREFIX}" \
      -DTCLMAIN=OFF \
      -DPYTHON3=OFF \
      -B "${BUILDDIR}/symdiff" \
      -S "${BASEDIR}/external/symdiff"

cmake --build ${BUILDDIR}/symdiff --config Release --parallel 4 -- //nologo //verbosity:minimal

mkdir -p ${BUILDDIR}/devsim
cmake -G "${GENERATOR}" \
    -A ${AOPTION} \
    -T ${TOOLSET} \
    -DDEVSIM_EXTENDED_PRECISION=ON \
    -DEIGEN_DENSE_EXTENDED=ON \
    -DDEVSIM_CPP_BIN_FLOAT=ON \
    -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
    -DMKL_PARDISO=ON \
    -DPYTHON3=ON \
    -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
    -DPYTHON3_ARCHIVE=${PYTHON3_ARCHIVE} \
    -B ${BUILDDIR}/devsim \
    -S ${BASEDIR}
cmake --build ${BUILDDIR}/devsim --config Release --parallel 4 -- //nologo //verbosity:minimal

cd "${BASEDIR}/dist"
bash package_appveyor.sh ${PACKAGE_NAME}

cd "${BASEDIR}/dist/${PACKAGE_NAME}"
#TODO: debug
echo "$PWD"
cp -f ../bdist_wheel/setup.* .
${PIP_BIN} wheel .
mv *.whl ..
