DEVSIM_CONFIG="osx_10.9"
CC=/usr/bin/gcc
CXX=/usr/bin/g++
CMAKE_CXX_FLAGS=""
for TYPE in debug release; do
  for ARCH in x86_64; do
  #for ARCH in i386 x86_64; do
    NAME=osx_${ARCH}_${TYPE}
    mkdir ${NAME}
    (cd $NAME; cmake \
      -DCMAKE_BUILD_TYPE=${TYPE} \
      -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_C_COMPILER=${CC} \
      -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
      -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS}" \
      ..)
  done
done
