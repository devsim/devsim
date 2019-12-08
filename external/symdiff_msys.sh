# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# Note: run this file using bash

ANACONDA="C:/Users/jsanchez/Miniconda3/envs/msys2"
CMAKE=$(cygpath -w ${ANACONDA}/Library/bin/cmake.exe)
CXX="C:/msys64/mingw64/bin/g++.exe"
PYTHON3_ARCHIVE=$(cygpath -w "${ANACONDA}/libs/python3.lib")
PYTHON3_INCLUDE=$(cygpath -w "${ANACONDA}/include")
PYTHON3_BIN=$(cygpath -w "${PYTHON3_BASE}/bin/python")

GENERATOR="MSYS Makefiles"
BUILDDIR="msys"

libpath=`/usr/bin/cygpath -w $PWD/lib`

mkdir -p ${BUILDDIR}
(cd ${BUILDDIR}; "$CMAKE" -G "${GENERATOR}" -DSYMDIFF_CONFIG=${SYMDIFF_CONFIG} -DTCLMAIN=OFF -DPYTHON3=ON \
        -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
        -DPYTHON3_ARCHIVE=${PYTHON3_ARCHIVE} \
	-DCMAKE_CXX_COMPILER=${CXX} \
..)

#echo $libpath
# TODO: fix to use conda activate
/usr/bin/mkdir -p bin
/usr/bin/cat << EOF > bin/symdiff_py3.bat
@setlocal
@echo off
SET PYTHONIOENCODING=utf-8
SET PYTHONPATH=$libpath
python %*
EOF
/usr/bin/chmod +x bin/symdiff_py3.bat

