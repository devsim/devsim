# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# Note: run this file using bash
set -e
set -u

ANACONDA_PATH=${CONDA_PREFIX}
CMAKE=$(cygpath -w ${ANACONDA_PATH}/Library/bin/cmake.exe)

GENERATOR="MSYS Makefiles"
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

