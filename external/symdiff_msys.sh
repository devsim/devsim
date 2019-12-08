# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# Note: run this file using bash

ANACONDA_PATH=${CONDA_PREFIX}
CMAKE=$(cygpath -w ${ANACONDA_PATH}/Library/bin/cmake.exe)

GENERATOR="MSYS Makefiles"
CXX=g++
PYTHON3_ARCHIVE=$(cygpath -w "${ANACONDA_PATH}/libs/python3.lib")
PYTHON3_INCLUDE=$(cygpath -w "${ANACONDA_PATH}/include")
PYTHON3_BIN=$(cygpath -w "${PYTHON3_BASE}/bin/python")
libpath=`/usr/bin/cygpath -w $PWD/lib`
for TYPE in debug release; do
ARCH=`uname -m`
    NAME=msys_${ARCH}_${TYPE}
    mkdir ${NAME}
    (cd $NAME; ${CMAKE} \
	-G "${GENERATOR}" \
	-DCMAKE_BUILD_TYPE=${TYPE} \
	-DCMAKE_CXX_COMPILER=${CXX} \
	-DTCLMAIN=OFF \
	-DPYTHON3=ON \
	-DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
	-DPYTHON3_ARCHIVE=${PYTHON3_ARCHIVE} \
      ..)
#  done
done

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

