#!/bin/bash
set -e
for ARCH in x86_64; do
PLATFORM=osx
SRC_DIR=../${PLATFORM}_${ARCH}_release/src/main
SRC_BIN=${SRC_DIR}/devsim_py
DIST_DIR=devsim_${PLATFORM}_${ARCH}
DIST_BIN=${DIST_DIR}/bin
DIST_DATE=`date +%Y%m%d`
DIST_VER=${DIST_DIR}_${DIST_DATE}

# make the bin directory and copy binary in
mkdir -p ${DIST_BIN}
# Assume libstdc++ is a standard part of the system
#http://developer.apple.com/library/mac/#documentation/DeveloperTools/Conceptual/CppRuntimeEnv/Articles/CPPROverview.html
cp ${SRC_BIN} ${DIST_DIR}/bin/devsim

# strip unneeded symbols
strip -arch all -u -r ${DIST_DIR}/bin/devsim
# keep a copy of unstripped binary
cp ${SRC_BIN} ${DIST_VER}_unstripped


mkdir -p ${DIST_DIR}/doc
cp ../doc/devsim.pdf ${DIST_DIR}/doc
for i in INSTALL NOTICE LICENSE COPYING RELEASE; do
cp ../$i ${DIST_DIR}
done


#### Python files and the examples
for i in python_packages examples testing
do
#local_files=`git status --no-ignore ../${i} | wc -l`
#if [ $local_files -ne 0 ]; then
#echo "!!!!!!!!!!!!!!!!!!! non repository files in $i"
#exit 1
#fi
(cd $i; git clean -f -d -x)
rsync -aP --delete ../$i ${DIST_DIR}
done

# copy the shared binary
#mkdir -p ${DIST_DIR}/lib/shared
#\cp -a /usr/lib/libstdc++*dylib devsim/lib/shared

echo "Available from:\
http://www.github.com/devsim/devsim" > ${DIST_DIR}/VERSION
git rev-parse --verify HEAD >> ${DIST_DIR}/VERSION

tar czf ${DIST_VER}.tgz ${DIST_DIR}
done
