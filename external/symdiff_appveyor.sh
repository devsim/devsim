# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# Note: run this file using bash
set -e
SYMDIFF_CONFIG="appveyor"

GENERATOR="$1"
AOPTION="$2"
BUILDDIR="$3"
CONDA_PREFIX="$4"

mkdir -p ${BUILDDIR}
(cd ${BUILDDIR}; cmake -G "${GENERATOR}" -A "${AOPTION}" -DSYMDIFF_CONFIG="${SYMDIFF_CONFIG}" -DANACONDA_PATH="${CONDA_PREFIX}" -DTCLMAIN=OFF -DPYTHON3=ON ..)

libpath=`cygpath -w $PWD/lib`

#echo $libpath
# TODO: fix to use conda activate
mkdir -p bin
cat << EOF > bin/symdiff_py3.bat
@setlocal
@echo off
SET PATH=${python3base};${python3base}\\Library\\bin;%PATH%
SET PYTHONIOENCODING=utf-8
SET PYTHONPATH=$libpath
python %*
EOF
/usr/bin/chmod +x bin/symdiff_py3.bat
