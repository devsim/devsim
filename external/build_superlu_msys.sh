
set -e
ANACONDA_PATH=${CONDA_PREFIX}
CMAKE=$(cygpath -w ${ANACONDA_PATH}/Library/bin/cmake.exe)
MAKE=make

GENERATOR="MSYS Makefiles"
BUILDDIR="SuperLU_4.3/msys"

(\
bsdtar xzf superlu_4.3.tar.gz && \
mkdir -p ${BUILDDIR} && \
cd ${BUILDDIR} && \
"${CMAKE}" -G "${GENERATOR}" -DCMAKE_BUILD_TYPE=RELEASE .. && \
"${MAKE}" -j2 \
)
