
set -e

GENERATOR="$1"
AOPTION="$2"
BUILDDIR="SuperLU_4.3/$3"

(\
bsdtar xzf superlu_4.3.tar.gz && \
mkdir -p ${BUILDDIR} && \
cd ${BUILDDIR} && \
cmake -G "${GENERATOR}" -A "${AOPTION}" .. && \
cmake --build . --config Release -- //m //nologo //verbosity:minimal \
)

