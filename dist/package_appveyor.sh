#!/bin/bash
set -e
set -u
set -o pipefail
set -x
if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
  echo "usage: package_macos.sh directory [abi3|abi3t]"
  exit 2;
fi
ABI=${2:-abi3}
case "${ABI}" in
  abi3|abi3t) ;;
  *) echo "unsupported wheel ABI: ${ABI}" >&2; exit 2 ;;
esac


for ARCH in win32 win64 clangwin64; do
PLATFORM=windows
DEVSIM_SRC_DIR=../${ARCH}/devsim/src/main/Release

# skip this directory if it was not built
if [ ! -d "${DEVSIM_SRC_DIR}" ]; then
  continue
fi

DIST_DIR=$1
DIST_BIN=${DIST_DIR}/bin
DIST_LIB=${DIST_DIR}/lib
DIST_PYDLL=${DIST_LIB}/devsim
DIST_VER=${DIST_DIR}

DLL_EXT=dll
PYDLL_EXT=${ABI}.pyd
# DO NOT HAVE TRAILING SLASHES!
SYMDIFF_DOCUMENTATION_DIR=../external/symdiff/doc
UMFPACK_LIBRARY_FILE=../${ARCH}/umfpack_lgpl/umfpack_lgpl.${DLL_EXT}

# make the bin directory and copy binary in
mkdir -p ${DIST_DIR}
mkdir -p ${DIST_BIN}
mkdir -p ${DIST_LIB}

cp -R ../lib/devsim ${DIST_LIB}
cp -v ${DEVSIM_SRC_DIR}/devsim_py3_${PYDLL_EXT} ${DIST_PYDLL}/devsim_py3.pyd

mkdir -p ${DIST_DIR}/doc
cp ../doc/devsim.pdf ${DIST_DIR}/doc
cp ${SYMDIFF_DOCUMENTATION_DIR}/symdiff.pdf ${DIST_DIR}/doc

for i in ../*.md ../CITATION.cff ../NOTICE ../LICENSE; do
cp "$i" "${DIST_DIR}"
done


#### Python files and the examples
for i in examples testing
do
(cd ../$i; git clean -f -d -x )
cp -R ../$i ${DIST_DIR}
done

# Copy UMFPACK DLL
cp -vf ${UMFPACK_LIBRARY_FILE} ${DIST_PYDLL}/umfpack

COMMIT=`git rev-parse --verify HEAD`
cat <<EOF > ${DIST_DIR}/VERSION
Source available from:
https://github.com/devsim/devsim
commit ${COMMIT}
EOF
done

