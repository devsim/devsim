# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/
TCL_BIN="/usr/bin/tclsh8.5"

PYTHON3_ARCHIVE="/usr/lib/x86_64-linux-gnu/libpython3.8m.so"
PYTHON3_INCLUDE="/usr/include/python3.8m"
PYTHON3_BIN="/usr/bin/python3"
for TYPE in debug release; do
ARCH=`uname -m`
#  for ARCH in i386 x86_64; do
    NAME=linux_${ARCH}_${TYPE}
    mkdir ${NAME}
    (cd $NAME; ${CMAKE} \
        -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
        -DCMAKE_BUILD_TYPE=${TYPE} \
        -DCMAKE_CXX_COMPILER=${CXX} \
        -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
        -DPYTHON3_ARCHIVE=${PYTHON3_ARCHIVE} \
        -DPYTHON3_BIN=${PYTHON3_BIN} \
        -DPYTHON3=ON \
        -DTCLMAIN=OFF \
        ..)
#  done
done
mkdir -p bin
cat << EOF > bin/symdiff
#!/bin/bash
set -e
progname="\$0"
curdir=\`dirname "\$progname"\`
export PYTHONPATH=\${curdir}/../lib
${PYTHON_BIN} \$*
EOF
chmod +x bin/symdiff

cat << EOF > bin/symdiff_py3
#!/bin/bash
set -e
progname="\$0"
curdir=\`dirname "\$progname"\`
export PYTHONPATH=\${curdir}/../lib
${PYTHON3_BIN} \$*
EOF
chmod +x bin/symdiff_py3

cat << EOF > bin/symdiff_tcl
#!/bin/bash
set -e
progname="\$0"
curdir=\`dirname "\$progname"\`
export TCLLIBPATH=\${curdir}/../lib
${TCL_BIN} \$*
EOF
chmod +x bin/symdiff_tcl

