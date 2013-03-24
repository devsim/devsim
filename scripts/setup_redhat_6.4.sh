DEVSIM_CONFIG=redhat_6.4
for TYPE in debug release; do
ARCH=`uname -m`
#  for ARCH in i386 x86_64; do
    NAME=linux_${ARCH}_${TYPE}
    mkdir ${NAME}
    (cd $NAME; cmake -DCMAKE_BUILD_TYPE=${TYPE} -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc .. -DDEVSIM_CONFIG=${DEVSIM_CONFIG})
#  done
done
