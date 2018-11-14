
CMAKE=/cygdrive/C/Program\ Files\ \(x86\)/CMake/bin/cmake.exe
DEVSIM_CONFIG=appveyor
#mkdir -p win32
#(cd win32; "$CMAKE" -G "Visual Studio 14" -DDEVSIM_CONFIG=${DEVSIM_CONFIG} ..)
if [ "$1" = x86 ]; then
GENERATOR="Visual Studio 15 2017"
BUILDDIR="win32"
fi
if [ "$1" = x64 ]; then
GENERATOR="Visual Studio 15 2017 Win64"
BUILDDIR="win64"
fi
/usr/bin/mkdir -p ${BUILDDIR}
(cd ${BUILDDIR}; "$CMAKE" -G "${GENERATOR}" -DDEVSIM_CONFIG=${DEVSIM_CONFIG} -DTCLMAIN=ON -DPYTHON3=ON ..)

