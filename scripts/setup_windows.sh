CMAKE=/cygdrive/C/Program\ Files\ \(x86\)/CMake/bin/cmake.exe
DEVSIM_CONFIG=win32
#mkdir -p win32/src/main/Release
#mkdir -p win32/src/main/Debug
mkdir -p win32
(cd win32; "$CMAKE" -G "Visual Studio 12" -DDEVSIM_CONFIG=${DEVSIM_CONFIG} ..)
#rsync -rv --exclude .svn lib win32/src/main/
#cp -v ../../externallib_win32/AMD/acml4.4.0/ifort32/lib/*.dl4.4.0/ifort64/lib/*.dll win64/src/main/Debug
