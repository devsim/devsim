
set -e
ANACONDA_PATH=${CONDA_PREFIX}
CMAKE=$(cygpath -w ${ANACONDA_PATH}/Library/bin/cmake.exe)
MAKE=make

GENERATOR="MSYS Makefiles"
BUILDDIR="superlu/msys"

(\
mkdir -p ${BUILDDIR} && \
cd ${BUILDDIR} && \
"${CMAKE}" -G "${GENERATOR}" -DCMAKE_BUILD_TYPE=RELEASE -Denable_complex=OFF -Denable_single=OFF -Denable_doc=OFF -Denable_tests=OFF -DXSDK_ENABLE_Fortran=OFF -DBLAS_FOUND=ON .. && \
"${MAKE}" -j2 \
)

