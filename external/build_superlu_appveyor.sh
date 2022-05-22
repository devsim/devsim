
set -e

GENERATOR="$1"
AOPTION="$2"
BUILDDIR="superlu/$3"

(\
mkdir -p ${BUILDDIR} && \
cd ${BUILDDIR} && \
cmake -G "${GENERATOR}" -A "${AOPTION}" -DCMAKE_C_FLAGS="/fp:strict" -Denable_complex=OFF -Denable_single=OFF -Denable_doc=OFF -Denable_tests=OFF -DXSDK_ENABLE_Fortran=OFF -DBLAS_FOUND=ON .. && \
cmake --build . --config Release -- //m //nologo //verbosity:minimal \
)

