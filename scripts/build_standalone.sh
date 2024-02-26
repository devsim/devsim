#!/bin/bash
set -e
set -u

export CMAKE=cmake
export CMAKE_CXX_FLAGS=""
export CC=clang
export CXX=clang++
export PYTHON3_BIN=python
export PYTHON3_INCLUDE=$(python -c "from sysconfig import get_paths as gp; print(gp()['include'])")
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
      -DDEVSIM_EXTENDED_PRECISION=ON \
      -DDEVSIM_CPP_BIN_FLOAT=ON \
      -DEIGEN_DENSE_EXTENDED=ON \
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
mkdir -p devsim
cp -v bundle/devsim/src/main/devsim_py3.so devsim
cp -v dist/bdist_wheel/setup.cfg .
cp -v dist/dist_standalone/setup.py .
cp -v dist/dist_standalone/__init__.py devsim
rsync -rvP python_packages devsim
