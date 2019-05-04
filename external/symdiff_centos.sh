# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/
CMAKE="cmake"
CMAKE_CXX_FLAGS=""
CXX="/opt/rh/devtoolset-6/root/usr/bin/g++"
TCL_ARCHIVE="/usr/lib64/libtclstub8.5.a"
TCL_INCLUDE="/usr/bin/include"
TCL_BIN="/usr/bin/tclsh8.5"
PYTHON_BASE="${HOME}/anaconda/envs/python27_devsim_build"
PYTHON_ARCHIVE="${PYTHON_BASE}/lib/libpython2.7.so"
PYTHON_INCLUDE="${PYTHON_BASE}/include/python2.7"
PYTHON_BIN="${PYTHON_BASE}/bin/python"
PYTHON3_BASE="${HOME}/anaconda/envs/python37_devsim_build"
PYTHON3_ARCHIVE="${PYTHON3_BASE}/lib/libpython3.7m.so"
PYTHON3_INCLUDE="${PYTHON3_BASE}/include/python3.7m"
PYTHON3_BIN="${PYTHON3_BASE}/bin/python"
for TYPE in debug release; do
ARCH=`uname -m`
#  for ARCH in i386 x86_64; do
    NAME=linux_${ARCH}_${TYPE}
    mkdir ${NAME}
    (cd $NAME; ${CMAKE} \
        -DCMAKE_CXX_FLAGS:STRING="${CMAKE_CXX_FLAGS}" \
        -DCMAKE_BUILD_TYPE=${TYPE} \
        -DCMAKE_CXX_COMPILER=${CXX} \
        -DPYTHON_INCLUDE=${PYTHON_INCLUDE} \
        -DPYTHON_ARCHIVE=${PYTHON_ARCHIVE} \
        -DPYTHON_BIN=${PYTHON_BIN} \
        -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
        -DPYTHON3_ARCHIVE=${PYTHON3_ARCHIVE} \
        -DPYTHON3_BIN=${PYTHON3_BIN} \
        -DTCL_INCLUDE=${TCL_INCLUDE} \
        -DTCL_ARCHIVE=${TCL_ARCHIVE} \
        -DTCL_BIN=${TCL_BIN} \
        -DPYTHON3=ON \
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

