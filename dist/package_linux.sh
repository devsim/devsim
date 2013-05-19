#!/bin/bash
set -e
ARCH=`uname -m`
PLATFORM=linux
SRC_DIR=../${PLATFORM}_${ARCH}_release/src/main
SRC_BIN=${SRC_DIR}/devsim_py
DIST_DIR=devsim_${PLATFORM}_${ARCH}
DIST_BIN=${DIST_DIR}/bin
DIST_DATE=`date +%Y%m%d`
DIST_VER=${DIST_DIR}_${DIST_DATE}

# make the bin directory and copy binary in
mkdir -p ${DIST_BIN}
# we need the wrapper script for libstdc++
cp ${SRC_BIN} ${DIST_DIR}/bin/devsim.bin
cp devsim.sh ${DIST_DIR}/bin/devsim
chmod +x ${DIST_DIR}/bin/devsim
# strip unneeded symbols
strip --strip-unneeded ${DIST_DIR}/bin/devsim.bin
# keep a copy of unstripped binary
cp ${SRC_BIN} ${DIST_VER}_unstripped


mkdir -p ${DIST_DIR}/doc
cp ../doc/devsim.pdf ${DIST_DIR}/doc


#### Python files and the examples
for i in python_packages examples
do
#local_files=`git status --no-ignore ../${i} | wc -l`
#if [ $local_files -ne 0 ]; then
#echo "!!!!!!!!!!!!!!!!!!! non repository files in $i"
#exit 1
#fi
rsync -aP --delete ../$i ${DIST_DIR}
done

# copy the shared binary
#mkdir -p ${DIST_DIR}/lib/shared
#\cp -a /usr/lib/libstdc++.so* ${DIST_DIR}/lib/shared


tar czf ${DIST_VER}.tgz ${DIST_DIR}

