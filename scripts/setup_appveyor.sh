
CMAKE=cmake
DEVSIM_CONFIG=appveyor
#mkdir -p win32
#(cd win32; "$CMAKE" -G "Visual Studio 14" -DDEVSIM_CONFIG=${DEVSIM_CONFIG} ..)
/usr/bin/mkdir -p win64
(cd win64; "$CMAKE" -G "Visual Studio 15 2017 Win64" -DDEVSIM_CONFIG=${DEVSIM_CONFIG} -DTCLMAIN=ON ..)
#(cd win64; "$CMAKE" -G "Visual Studio 15 2017 Win64" -DDEVSIM_CONFIG=${DEVSIM_CONFIG} -DTCLMAIN=ON ..)
