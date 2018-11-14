#!/bin/bash
set -e
if ! [ $1 ]; then
  echo "must specify dir name"
  exit 2;
fi


for ARCH in win32 win64; do
PLATFORM=windows
SRC_DIR=../${ARCH}/src/main/Release

# skip this directory if it was not built
if [ ! -d "${SRC_DIR}" ]; then
  continue
fi

#DIST_DIR=devsim_${PLATFORM}_${ARCH}
DIST_DIR=$1
#DIST_DIR=$1_${ARCH}
DIST_BIN=${DIST_DIR}/bin
DIST_LIB=${DIST_DIR}/lib
DIST_PYDLL=${DIST_LIB}/devsim
DIST_VER=${DIST_DIR}
######MT_EXE="/cygdrive/c/Program Files (x86)/Windows Kits/8.1/bin/x64/mt.exe"


# make the bin directory and copy binary in
mkdir -p ${DIST_BIN}
mkdir -p ${DIST_DIR}
mkdir -p ${DIST_PYDLL}

cp -v ${SRC_DIR}/devsim_py27.pyd ${DIST_PYDLL}
cp -v ${SRC_DIR}/devsim_py36.pyd ${DIST_PYDLL}
cp -v ${SRC_DIR}/devsim_py37.pyd ${DIST_PYDLL}
cp -v ${SRC_DIR}/devsim_tcl.exe ${DIST_BIN}
cp -v __init__.py ${DIST_PYDLL}

##### update the manifest
##### this is not necessary for pyd files
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
for i in examples testing
do
(cd ../$i; git clean -f -d -x )
cp -R ../$i ${DIST_DIR}
done
cp -R ../python_packages ${DIST_PYDLL}



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

