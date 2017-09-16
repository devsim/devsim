DEVSIM_CONFIG="ubuntu_12.04_intel"
CC=icc
CXX=icpc
CMAKE_C_FLAGS="-fp-model precise -fp-model source"
CMAKE_CXX_FLAGS="${CMAKE_C_FLAGS}"
for TYPE in debug release; do
ARCH=`uname -m`
#  for ARCH in i386 x86_64; do
    NAME=linux_${ARCH}_${TYPE}
    mkdir -p ${NAME}
    (cd $NAME; cmake \
      -DCMAKE_BUILD_TYPE=${TYPE} \
      -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_C_COMPILER=${CC} \
      -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
      -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS}" \
      -DCMAKE_C_FLAGS="${CMAKE_C_FLAGS}" \
      -DBLAS_ARCHIVE="${BLAS_ARCHIVE}" \
      -DMKLROOT="${MKLROOT}" \
      ..)
#  done
done
