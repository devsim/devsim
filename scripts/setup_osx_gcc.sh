DEVSIM_CONFIG="osx_gcc"
#CC=/usr/local/bin/gcc-8
#CXX=/usr/local/bin/g++-8
if [ -z "${CXX}" ]
then
echo "Setting CC and CXX from values needing to match build_macos"
export CC=/usr/local/Cellar/gcc/8.2.0/bin/gcc-8;
export CXX=/usr/local/Cellar/gcc/8.2.0/bin/g++-8
fi
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
      -DDEVSIM_EXTENDED_PRECISION=ON \
      -DMKL_PARDISO=ON \
      -DPYTHON3=ON \
      ..)
  done
done
