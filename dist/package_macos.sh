#!/bin/bash
set -e
set -x
if ! [ $1 ]; then
  echo "must specify dir name"
  exit 2;
fi

ARCH=x86_64
PLATFORM=osx
DEVSIM_SRC_DIR=../${PLATFORM}_${ARCH}_release/src/main
DIST_DIR=$1
DIST_BIN=${DIST_DIR}/bin
DIST_LIB=${DIST_DIR}/lib
DIST_PYDLL=${DIST_LIB}/devsim
DIST_VER=${DIST_DIR}

DLL_EXT=dylib
PYDLL_EXT=so
# DO NOT HAVE TRAILING SLASHES!
SYMDIFF_DOCUMENTATION_DIR=../external/symdiff/doc
UMFPACK_LIBRARY_FILE=../external/umfpack_lgpl/build/libumfpack_lgpl.${DLL_EXT}

# make the bin directory and copy binary in
mkdir -p ${DIST_BIN}
mkdir -p ${DIST_DIR}
mkdir -p ${DIST_PYDLL}

cp -v ${DEVSIM_SRC_DIR}/devsim_py3.${PYDLL_EXT} ${DIST_PYDLL}
cp -v __init__.py ${DIST_PYDLL}
cp -v __init__.pyi ${DIST_PYDLL}


mkdir -p ${DIST_DIR}/doc
cp ../doc/devsim.pdf ${DIST_DIR}/doc
cp ${SYMDIFF_DOCUMENTATION_DIR}/symdiff.pdf ${DIST_DIR}/doc

for i in *.md CITATION.cff NOTICE LICENSE; do
cp ../$i ${DIST_DIR}
done


#### Python files and the examples
for i in examples testing
do
(cd ../$i; git clean -f -d -x )
cp -R ../$i ${DIST_DIR}
done
cp -R ../python_packages ${DIST_PYDLL}
cp -R ../umfpack ${DIST_PYDLL}/umfpack

# Copy UMFPACK DLL
cp -vf ${UMFPACK_LIBRARY_FILE} ${DIST_PYDLL}/umfpack


COMMIT=`git rev-parse --verify HEAD`
cat <<EOF > ${DIST_DIR}/VERSION
Source available from:
https://github.com/devsim/devsim
commit ${COMMIT}
EOF

