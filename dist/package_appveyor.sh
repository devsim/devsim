#!/bin/bash
set -e
set -x
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
DIST_BIN=${DIST_DIR}/bin
DIST_LIB=${DIST_DIR}/lib
DIST_PYDLL=${DIST_LIB}/devsim
DIST_VER=${DIST_DIR}
# DO NOT HAVE TRAILING SLASHES!
SYMDIFF_LIBRARY_DIR=../external/symdiff/lib/symdiff
SYMDIFF_EXAMPLES_DIR=../external/symdiff/examples
SYMDIFF_DOCUMENTATION_DIR=../external/symdiff/doc
UMFPACK_LIBRARY_FILE=../external/umfpack_lgpl/${ARCH}/umfpack_lgpl.dll

# make the bin directory and copy binary in
mkdir -p ${DIST_BIN}
mkdir -p ${DIST_DIR}
mkdir -p ${DIST_PYDLL}

cp -v ${SRC_DIR}/devsim_py3.pyd ${DIST_PYDLL}
#cp -v ${SRC_DIR}/devsim_tcl.exe ${DIST_BIN}
cp -v __init__.py ${DIST_PYDLL}

# Intel MKL
#cp -v ${CONDA_PREFIX}/Library/bin/mkl_rt.1.dll ${DIST_PYDLL}

# goes to lib/symdiff
cp -R ${SYMDIFF_LIBRARY_DIR} ${DIST_LIB}

##### update the manifest
##### this is not necessary for pyd files
#####(cd ${DIST_BIN} &&
#####"${MT_EXE}" -inputresource:"c:\\Miniconda-x64\\python.exe;#1" -out:devsim.exe.manifest &&
#####"${MT_EXE}" -manifest devsim.exe.manifest -outputresource:"devsim.exe;#1"
#####)

mkdir -p ${DIST_DIR}/doc
cp ../doc/devsim.pdf ${DIST_DIR}/doc
cp ${SYMDIFF_DOCUMENTATION_DIR}/symdiff.pdf ${DIST_DIR}/doc

for i in INSTALL.md NOTICE LICENSE README.md CHANGES.md install.py; do
cp ../$i ${DIST_DIR}
done


#### Python files and the examples
for i in examples testing
do
#issue with git clean behavior on appveyor
#(cd ../$i; git clean -f -d -x )
cp -R ../$i ${DIST_DIR}
done
cp -R ../python_packages ${DIST_PYDLL}
cp -R ../umfpack ${DIST_PYDLL}/umfpack

# Copy UMFPACK DLL
cp -v ${UMFPACK_LIBRARY_FILE} ${DIST_PYDLL}/umfpack

mkdir -p ${DIST_DIR}/examples/symdiff
# add trailing slash for rsync
cp -R ${SYMDIFF_EXAMPLES_DIR}/* ${DIST_DIR}/examples/symdiff

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

