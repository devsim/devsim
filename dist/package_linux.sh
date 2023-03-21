#!/bin/bash
set -e
if ! [ $1 ]; then
  echo "must specify dir name"
  exit 2;
fi


for ARCH in `uname -m`; do
PLATFORM=linux
SRC_DIR=../${PLATFORM}_${ARCH}_release/src/main
DIST_DIR=$1
DIST_BIN=${DIST_DIR}/bin
DIST_LIB=${DIST_DIR}/lib
DIST_PYDLL=${DIST_LIB}/devsim
DIST_VER=${DIST_DIR}
# DO NOT HAVE TRAILING SLASHES!
SYMDIFF_LIBRARY_DIR=../external/symdiff/lib/symdiff
SYMDIFF_EXAMPLES_DIR=../external/symdiff/examples
SYMDIFF_DOCUMENTATION_DIR=../external/symdiff/doc
UMFPACK_LIBRARY_FILE=../external/umfpack_lgpl/build/libumfpack_lgpl.so

# make the bin directory and copy binary in
# we need the wrapper script for libstdc++
#cp devsim.sh ${DIST_DIR}/bin/devsim
#chmod +x ${DIST_DIR}/bin/devsim
mkdir -p ${DIST_BIN}
mkdir -p ${DIST_DIR}
mkdir -p ${DIST_PYDLL}

cp -v ${SRC_DIR}/devsim_py3.so ${DIST_PYDLL}
#cp -v ${SRC_DIR}/devsim_tcl ${DIST_BIN}
cp -v __init__.py ${DIST_PYDLL}

# goes to lib/symdiff
rsync -aqP --delete ${SYMDIFF_LIBRARY_DIR} ${DIST_LIB}
# strip unneeded symbols
#strip --strip-unneeded ${DIST_DIR}/bin/$i
#done
# keep a copy of unstripped binary
#cp ${SRC_BIN} ${DIST_VER}_unstripped


mkdir -p ${DIST_DIR}/doc
cp ../doc/devsim.pdf ${DIST_DIR}/doc
cp ${SYMDIFF_DOCUMENTATION_DIR}/symdiff.pdf ${DIST_DIR}/doc

for i in INSTALL.md NOTICE LICENSE README.md CHANGES.md install.py; do
cp ../$i ${DIST_DIR}
done

#MKL library
# $ORIGIN rpath set in src/main/CMakeLists.txt
#cp "${CONDA_PREFIX}/lib/libmkl_rt.so.1" ${DIST_PYDLL}


#### Python files and the examples
for i in examples testing
do
(cd ../$i; git clean -f -d -x )
rsync -aqP --delete ../$i ${DIST_DIR}
done
rsync -aqP --delete ../python_packages ${DIST_PYDLL}
rsync -aqP --delete ../umfpack ${DIST_PYDLL}

# Copy UMFPACK DLL
cp -vf ${UMFPACK_LIBRARY_FILE} ${DIST_PYDLL}/umfpack

mkdir -p ${DIST_DIR}/examples/symdiff
# add trailing slash for rsync
rsync -aqP --delete ${SYMDIFF_EXAMPLES_DIR}/ ${DIST_DIR}/examples/symdiff

COMMIT=`git rev-parse --verify HEAD`
cat <<EOF > ${DIST_DIR}/VERSION
Package released as:
${DIST_VER}.tgz

Source available from:
http://github.com/devsim/devsim
commit ${COMMIT}
EOF
tar czvf ${DIST_VER}.tgz ${DIST_DIR}
done

