CMAKE=/cygdrive/C/Program\ Files\ \(x86\)/CMake/bin/cmake.exe
DEVSIM_CONFIG=win32
mkdir -p win32
(cd win32; "$CMAKE" -G "Visual Studio 12" -DDEVSIM_CONFIG=${DEVSIM_CONFIG} ..)
mkdir -p win64
(cd win64; "$CMAKE" -G "Visual Studio 12 Win64" -DDEVSIM_CONFIG=${DEVSIM_CONFIG} ..)
