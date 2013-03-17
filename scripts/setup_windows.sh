CMAKE=/cygdrive/C/Program\ Files\ \(x86\)/CMake\ 2.8/bin/cmake.exe

mkdir -p win32/src/main/Release
mkdir -p win32/src/main/Debug
(cd win32; "$CMAKE" -G "Visual Studio 10" ..)
rsync -rv --exclude .svn lib win32/src/main/
cp -v ../../externallib_win32/AMD/acml4.4.0/ifort32/lib/*.dll win32/src/main/Release
cp -v ../../externallib_win32/AMD/acml4.4.0/ifort32/lib/*.dll win32/src/main/Debug

mkdir -p win64/src/main/Release
mkdir -p win64/src/main/Debug
(cd win64; "$CMAKE" -G "Visual Studio 10 Win64" .)
rsync -rv --exclude .svn lib win64/src/main/
cp -v ../../externallib_win32/AMD/acml4.4.0/ifort64/lib/*.dll win64/src/main/Release
cp -v ../../externallib_win32/AMD/acml4.4.0/ifort64/lib/*.dll win64/src/main/Debug
