DEVSIM_CONFIG="ubuntu_18.04"
for TYPE in debug release; do
ARCH=`uname -m`
#  for ARCH in i386 x86_64; do
    NAME=linux_${ARCH}_${TYPE}
    mkdir ${NAME}
    (cd $NAME; cmake \
      -DCMAKE_BUILD_TYPE=${TYPE} \
      -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_C_COMPILER=${CC} \
      -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
      -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
      -DDEVSIM_EXTENDED_PRECISION=ON \
      -DPYTHON3=ON \
      -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
      -DEXPLICIT_MATH_LOAD=ON \
      -DMKL_PARDISO=ON \
      ..)
#  done
done
