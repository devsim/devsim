# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# Note: run this file using bash
CMAKE=/cygdrive/C/Program\ Files\ \(x86\)/CMake/bin/cmake.exe
SYMDIFF_CONFIG="appveyor"

if [ "$1" = x86 ]; then
GENERATOR="Visual Studio 15 2017"
BUILDDIR="win32"
python3base="c:\\Miniconda37"
fi
if [ "$1" = x64 ]; then
GENERATOR="Visual Studio 15 2017 Win64"
BUILDDIR="win64"
python3base="c:\\Miniconda37-x64"
fi

#/usr/bin/mkdir -p win32
#(cd win32; "$CMAKE" -G "Visual Studio 14" -DSYMDIFF_CONFIG=${SYMDIFF_CONFIG} ..)

/usr/bin/mkdir -p ${BUILDDIR}
(cd ${BUILDDIR}; "$CMAKE" -G "${GENERATOR}" -DSYMDIFF_CONFIG=${SYMDIFF_CONFIG} -DTCLMAIN=OFF -DPYTHON3=ON ..)
#(cd win64; "$CMAKE" -G "Visual Studio 14 Win64" -DSYMDIFF_CONFIG=${SYMDIFF_CONFIG} -DTCLMAIN=ON ..)

libpath=`/usr/bin/cygpath -w $PWD/lib`

#echo $libpath
# TODO: fix to use conda activate
/usr/bin/mkdir -p bin
/usr/bin/cat << EOF > bin/symdiff_py3.bat
@setlocal
@echo off
SET PATH=${python3base};${python3base}\\Library\\bin;%PATH%
SET PYTHONIOENCODING=utf-8
SET PYTHONPATH=$libpath
python %*
EOF
/usr/bin/chmod +x bin/symdiff_py3.bat

