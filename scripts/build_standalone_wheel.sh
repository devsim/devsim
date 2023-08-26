#!/bin/bash
set -e
set -u

export PIP_BIN=pip
${PIP_BIN} install wheel

if ! [ $1 ]; then
  echo "must specify dir name"
  exit 2;
fi


SRC_DIR=${PWD}/bundle/devsim/src/main
DIST_DIR=$1
DIST_BIN=${DIST_DIR}/bin
DIST_LIB=${DIST_DIR}/lib
DIST_PYDLL=${DIST_LIB}/devsim
DIST_VER=${DIST_DIR}

# make the bin directory and copy binary in
# we need the wrapper script for libstdc++
#cp devsim.sh ${DIST_DIR}/bin/devsim
#chmod +x ${DIST_DIR}/bin/devsim
mkdir -p ${DIST_BIN}
mkdir -p ${DIST_DIR}
mkdir -p ${DIST_PYDLL}

cp -v ${SRC_DIR}/devsim_py3.so ${DIST_PYDLL}
cp -v dist/__init__.py ${DIST_PYDLL}

mkdir -p ${DIST_DIR}/doc
cp doc/devsim.pdf ${DIST_DIR}/doc

for i in INSTALL.md NOTICE LICENSE README.md CHANGES.md install.py; do
cp $i ${DIST_DIR}
done

#### Python files and the examples
for i in examples testing
do
(cd $i; git clean -f -d -x )
rsync -aqP --delete $i ${DIST_DIR}
done
rsync -aqP --delete python_packages ${DIST_PYDLL}

COMMIT=`git rev-parse --verify HEAD`
cat <<EOF > ${DIST_DIR}/VERSION
Package released as:
${DIST_VER}.tgz

Source available from:
http://github.com/devsim/devsim
commit ${COMMIT}
EOF
tar czvf ${DIST_VER}.tgz ${DIST_DIR}

cp -f dist/bdist_wheel_standalone/setup.py ${1}/
cp -f dist/bdist_wheel/setup.cfg ${1}/
(cd ${1} && ${PIP_BIN} wheel .)
(cd ${1} && mv -v *.whl ..)

