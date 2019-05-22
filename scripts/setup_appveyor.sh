
CMAKE="/cygdrive/C/Program Files (x86)/CMake/bin/cmake.exe"
DEVSIM_CONFIG=appveyor

if [ "${1}" = x86 ]; then
GENERATOR="Visual Studio 15 2017"
BUILDDIR="win32"
fi
if [ "${1}" = x64 ]; then
GENERATOR="Visual Studio 15 2017 Win64"
BUILDDIR="win64"
fi
/usr/bin/mkdir -p ${BUILDDIR}
(cd ${BUILDDIR}; "$CMAKE" -G "${GENERATOR}" -DDEVSIM_CONFIG=${DEVSIM_CONFIG} -DPYTHON3=ON  -DMKL_PARDISO=ON ..)

