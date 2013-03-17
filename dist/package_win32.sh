#!/bin/bash
set -e
for ARCH in win32 win64; do
echo $ARCH
PLATFORM=windows
SRC_DIR=../${ARCH}/src/main/Release
SRC_BIN=${SRC_DIR}/devsim.exe
DIST_DIR=devsim_${ARCH}
DIST_BIN=${DIST_DIR}/bin
DIST_DATE=`date +%Y%m%d`
DIST_VER=${DIST_DIR}_${DIST_DATE}

# make the bin directory and copy binary in
mkdir -p ${DIST_BIN}
cp ${SRC_BIN} ${DIST_BIN}
cp ${SRC_DIR}/*.dll ${DIST_BIN}








mkdir -p ${DIST_DIR}/doc
cp ../manual/devsim.pdf ${DIST_DIR}/doc


#### Tcl library files and the examples
for i in lib examples
do
local_files=`svn status --no-ignore ../${i} | wc -l`
if [ $local_files -ne 0 ]; then
echo "!!!!!!!!!!!!!!!!!!! non repository files in $i"
exit 1
fi
rsync --exclude ".svn" -aP --delete ../$i ${DIST_DIR}
done



zip -r ${DIST_VER}.zip ${DIST_DIR}
done
