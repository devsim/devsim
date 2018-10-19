#!/bin/bash
set -e
if ! [ $1 ]; then
  echo "must specify dir name"
  exit 2;
fi


for ARCH in win64; do
PLATFORM=windows
SRC_DIR=../${ARCH}/src/main/Release
#DIST_DIR=devsim_${PLATFORM}_${ARCH}
DIST_DIR=$1
#DIST_DIR=$1_${ARCH}
DIST_BIN=${DIST_DIR}/bin
#DIST_DATE=`date +%Y%m%d`
DIST_VER=${DIST_DIR}
######MT_EXE="/cygdrive/c/Program Files (x86)/Windows Kits/8.1/bin/x64/mt.exe"

# debugging
ls -l ${SRC_DIR}/*.pyd ${SRC_DIR}/*.exe
# make the bin directory and copy binary in
mkdir -p ${DIST_BIN}
#cp ${SRC_DIR}/devsim_py.exe ${DIST_BIN}/devsim.exe
#cp ${SRC_DIR}/devsim_py3.exe ${DIST_BIN}/devsim_py3.exe
#cp ${SRC_DIR}/devsim_tcl ${DIST_BIN}/devsim_tcl.exe
for i in devsim_py27.pyd devsim_py36.pyd devsim_tcl.exe; do cp -v ${SRC_DIR}/$i ${DIST_BIN}; done
##### update the manifest
##### hopefully not necessary for pyd files
#####(cd ${DIST_BIN} &&
#####"${MT_EXE}" -inputresource:"c:\\Miniconda-x64\\python.exe;#1" -out:devsim.exe.manifest &&
#####"${MT_EXE}" -manifest devsim.exe.manifest -outputresource:"devsim.exe;#1"
#####)

mkdir -p ${DIST_DIR}/doc
cp ../doc/devsim.pdf ${DIST_DIR}/doc
for i in INSTALL NOTICE LICENSE RELEASE windows.txt; do
cp ../$i ${DIST_DIR}
done


#### Python files and the examples
for i in python_packages examples testing
do
(cd ../$i; git clean -f -d -x )
cp -R ../$i ${DIST_DIR}
done



COMMIT=`git rev-parse --verify HEAD`
cat <<EOF > ${DIST_DIR}/VERSION
Package released as:
${DIST_VER}.zip

Source available from:
http://www.github.com/devsim/devsim 
commit ${COMMIT}
EOF



/usr/bin/zip -r ${DIST_DIR}.zip ${DIST_DIR}
done

