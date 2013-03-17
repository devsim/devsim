for TYPE in debug release; do
  for ARCH in x86_64; do
    NAME=osx_${ARCH}_${TYPE}
    mkdir ${NAME}
    (cd $NAME; /usr/bin/cmake -DCMAKE_OSX_ARCHITECTURES=${ARCH} -DCMAKE_BUILD_TYPE=${TYPE} -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc ..; ln -s ../lib)
  done
done
