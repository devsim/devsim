
set -e
CMAKE="/cygdrive/C/Program Files (x86)/CMake/bin/cmake.exe"

if [ "${1}" = x86 ]; then
GENERATOR="Visual Studio 15 2017"
BUILDDIR="SuperLU_4.3/win32"
fi
if [ "${1}" = x64 ]; then
GENERATOR="Visual Studio 15 2017 Win64"
BUILDDIR="SuperLU_4.3/win64"
fi

(\
tar xzf superlu_4.3.tar.gz && \
mkdir -p ${BUILDDIR} && \
cd ${BUILDDIR} && \
"${CMAKE}" -G "${GENERATOR}" .. && \
"${CMAKE}" --build . --config Release -- /m /nologo /verbosity:minimal \
)

