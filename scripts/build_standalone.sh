#!/bin/bash
set -e
set -u

export CMAKE=cmake
export CMAKE_CXX_FLAGS=""
export CC=clang
export CXX=clang++
export PYTHON3_BIN=python3
export PIP_BIN=pip
${PIP_BIN} install wheel
export PYTHON3_INCLUDE=$(python3 -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

mkdir -p bundle/superlu

${CMAKE} -DCMAKE_BUILD_TYPE=RELEASE \
         -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
         -DCMAKE_C_COMPILER=${CC} \
         -Denable_complex=OFF \
         -Denable_single=OFF \
         -Denable_doc=OFF \
         -Denable_tests=OFF \
         -DXSDK_ENABLE_Fortran=OFF \
         -DBLAS_FOUND=OFF \
         -DCMAKE_C_FLAGS="-fvisibility=hidden -DAdd__" \
         -B bundle/superlu \
         -S external/superlu

(cd bundle/superlu && ${CMAKE} --build . -- -j8)

mkdir -p bundle/symdiff

${CMAKE} \
        -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
        -DCMAKE_BUILD_TYPE=RELEASE \
        -DCMAKE_CXX_COMPILER=${CXX} \
        -DPYTHON3=OFF \
        -DTCLMAIN=OFF \
        -DCOPY_PROJECT_DIR=${PWD}/bundle/symdiff \
        -B bundle/symdiff \
        -S external/symdiff

(cd bundle/symdiff && ${CMAKE} --build . -- -j8)

mkdir -p bundle/devsim

EXTERNAL_LIB=${PWD}/external
EIGEN_INCLUDE=${EXTERNAL_LIB}/eigen
FLEX=/usr/bin/flex
BISON=/usr/bin/bison
BOOST_INCLUDE="${EXTERNAL_LIB}/boostorg/config/include;${EXTERNAL_LIB}/boostorg/math/include;${EXTERNAL_LIB}/boostorg/multiprecision/include"
BOOST_DEFINITIONS="-DBOOST_MP_STANDALONE -DBOOST_MATH_STANDALONE"
SUPERLU_INCLUDE=${EXTERNAL_LIB}/superlu/SRC
SUPERLU_ARCHIVE="${PWD}/bundle/superlu/SRC/libsuperlu.a;${PWD}/bundle/superlu/CBLAS/libblas.a"
SYMDIFF_ARCHIVE=${PWD}/bundle/symdiff/lib/libsymdiff_dynamic.a
SYMDIFF_INCLUDE=${PWD}/bundle/symdiff/include

${CMAKE} \
      -DCMAKE_BUILD_TYPE=RELEASE \
      -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_C_COMPILER=${CC} \
      -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -Wall -fvisibility=hidden ${BOOST_DEFINITIONS}" \
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
      -DDEVSIM_EXTENDED_PRECISION=OFF \
      -DDEVSIM_CPP_BIN_FLOAT=OFF \
      -DEIGEN_DENSE_EXTENDED=OFF \
      -DEIGEN_DENSE_DOUBLE=ON \
      -DSUPERLU_PRECONDITIONER=ON \
      -DDEVSIM_LOAD_MATHLIBS=OFF \
      -DDEVSIM_MATERIALDB=OFF \
      -DVTKWRITER=OFF \
      -DMKL_PARDISO=OFF \
      -DPYTHON3=ON \
      -DPYTHON3_INCLUDE="${PYTHON3_INCLUDE}" \
      -DBOOST_INCLUDE="${BOOST_INCLUDE}" \
      -DEIGEN_INCLUDE="${EIGEN_INCLUDE}" \
      -DFLEX=${FLEX} \
      -DBISON=${BISON} \
      -DSYMDIFF_INCLUDE=${SYMDIFF_INCLUDE} \
      -DSYMDIFF_ARCHIVE=${SYMDIFF_ARCHIVE} \
      -DSUPERLU_INCLUDE=${SUPERLU_INCLUDE} \
      -DSUPERLU_ARCHIVE=${SUPERLU_ARCHIVE} \
      -S . \
      -B bundle/devsim

(cd bundle/devsim && ${CMAKE} --build . -- -j8)

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

cp -f dist/bdist_wheel_noumfpack/setup.* ${1}/
(cd ${1} && ${PIP_BIN} wheel .)
