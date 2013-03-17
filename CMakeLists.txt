PROJECT (DEVSIM)
#cmake_minimum_required(VERSION 2.6)
#
## This version strips the newline at the end
##EXEC_PROGRAM(uname  ARGS -m OUTPUT_VARIABLE ARCH)
## There should be a strip option in later versions of cmake (e.g. 2.4.4)
##EXECUTE_PROCESS(COMMAND uname -m OUTPUT_VARIABLE ARCH)
##MESSAGE("ARCH ${ARCH}")
#
#IF (WIN32)
#SET (CMAKE_CXX_WARNING_LEVEL 4)
#SET (FLEX "C:/cygwin/bin/flex.exe")
#SET (BISON "C:/cygwin/bin/bison.exe")
#SET (Boost_INCLUDE_DIRS "C:/Users/jsanchez/WindowsLibs/externallib_win32/boost_1_39_0") 
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
#ADD_DEFINITIONS(-DSTATIC_BUILD -D_USE_MATH_DEFINES)
##Math stuff
#SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:strict")
#SET (CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /fp:strict")
##ADD_DEFINITIONS(-DSTATIC_BUILD /fp:strict)
##SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
##SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT /NODEFAULTLIB:OLDNAMES")
#ELSE (WIN32)
#ADD_DEFINITIONS(-Wall -DTCL_THREADS -pthread)
##INCLUDE(FindBoost)
##INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
#SET (FLEX flex)
#SET (BISON bison)
#ENDIF (WIN32)
#
#
#SITE_NAME(HOSTNAME)
#MESSAGE("Hostname: ${HOSTNAME}")
#
## windows build
#IF (WIN32)
##IF (HOSTNAME STREQUAL "JSANCHEZ-PC")
#SET (THIRDPARTY c:/users/jsanchez/WindowsLibs/externallib_win32)
##ENDIF (HOSTNAME STREQUAL "JSANCHEZ-PC")
#ADD_DEFINITIONS(-DTCL_THREADS)
#SET (SUPERLULOCATE  ${THIRDPARTY}/SuperLU_4.0)
#SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
#SET (ZLIB_INCLUDE "${THIRDPARTY}/zlib-1.2.3")
#SET (SQLITE3_INCLUDE "${THIRDPARTY}/sqlite-amalgamation-3070701")
#SET (TCL_INCLUDE "${THIRDPARTY}/tcl8.5.9/generic")
##SET (BLAS_ARCHIVE  "${THIRDPARTY}/acml4.3.0/ifort32/lib/libacml_dll.lib")
##has linking issues missing ifconsol.lib
##SET (BLAS_ARCHIVE  "${THIRDPARTY}/acml4.3.0/ifort32/lib/libacml.lib")
#SET (PYTHON_INCLUDE C:/Python27/include)
#SET (PYTHON_ARCHIVE C:/Python27/libs/python27.lib)
#SET (SQLITE3_ARCHIVE "${THIRDPARTY}/sqlite-amalgamation-3070701/cmake/Release/sqlite3.lib")
#SET (TCL_STUB "${THIRDPARTY}/tcl8.5.9/win/Release_AMD64_VC9/tclstub85.lib")
#IF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
#SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/superlu.lib)
#SET (BLAS_ARCHIVE  "${THIRDPARTY}/AMD/acml4.4.0/ifort32/lib/libacml_dll.lib")
#SET (TCL_ARCHIVE "${THIRDPARTY}/tcl8.5.9/win/Release_VC9/tcl85tsx.lib")
##SET (SQLITE3_ARCHIVE "${THIRDPARTY}/tclsqlite3/Release/tclsqlite3.lib")
##SET (SQLITE3_ARCHIVE "${THIRDPARTY}/sqlite-3_6_16-tea/sqlite/Release/tclsqlite.lib")
#SET (ZLIB_ARCHIVE "${THIRDPARTY}/zlib-1.2.3/zlib_32.lib")
#
#ELSE (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
#SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/superlu_64.lib)
#SET (BLAS_ARCHIVE  "${THIRDPARTY}/AMD/acml4.4.0/ifort64/lib/libacml_dll.lib")
#SET (TCL_ARCHIVE "${THIRDPARTY}/tcl8.5.9/win/Release_AMD64_VC9/tcl85tsx.lib")
#SET (TCLSQLITE3_ARCHIVE "${THIRDPARTY}/sqlite-3_6_16-tea/sqlite_64/Release/tclsqlite.lib")
#SET (ZLIB_ARCHIVE "${THIRDPARTY}/zlib-1.2.3/zlib_64.lib")
#ENDIF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
#ENDIF (WIN32)
#
#

# Ubuntu 12.04 64 bit
IF (${DEVSIM_CONFIG} STREQUAL "ubuntu_12.04")
SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)
SET (PYTHON_INCLUDE /usr/include/python2.7)
SET (PYTHON_ARCHIVE /usr/lib/libpython2.7.so)
# use /opt/acml5.3.0/util/cpuid.exe to find support
# ACML 5.3 available at developer.amd.com
SET (BLAS_ARCHIVE /opt/acml5.3.0/gfortran64/lib/libacml.a /usr/lib/gcc/x86_64-linux-gnu/4.6/libgfortran.a)

SET (ZLIB_ARCHIVE /usr/lib/x86_64-linux-gnu/libz.a)
SET (ZLIB_INCLUDE /usr/include)

SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3.a)
SET (TCL_INCLUDE "/usr/include/tcl8.5")
SET (TCL_ARCHIVE "/usr/lib/libtcl8.5.a" -ldl)
SET (TCL_STUB /usr/lib/libtclstub8.5.a)
SET (PTHREAD_LIB -lpthread)
SET (SQLITE3_ARCHIVE /usr/lib/x86_64-linux-gnu/libsqlite3.a)
SET (SQLITE3_INCLUDE /usr/include)

SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_static.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
ENDIF (${DEVSIM_CONFIG} STREQUAL "ubuntu_12.04")

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
ADD_DEFINITIONS(-DTCL_THREADS -pthread -fvisibility=hidden)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF (${DEVSIM_CONFIG} STREQUAL "osx_10.6.8")
SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)
#Download python 2.7.3 from python.org
SET (PYTHON_INCLUDE /Library/Frameworks/Python.framework/Versions/2.7/Headers)
SET (PYTHON_ARCHIVE /Library/Frameworks/Python.framework/Versions/2.7/lib/libpython2.7.dylib)
SET (TCL_STUB /usr/lib/libtclstub8.5.a)
SET (BLAS_ARCHIVE  /usr/lib/libblas.dylib /usr/lib/libcblas.dylib /usr/lib/liblapack.dylib)
SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3_${CMAKE_OSX_ARCHITECTURES}.a)
SET (TCL_INCLUDE "/usr/include")
SET (TCL_ARCHIVE "/usr/lib/libtcl8.5.dylib" -ldl)
SET (SQLITE3_ARCHIVE /usr/lib/libsqlite3.dylib)
SET (SQLITE3_INCLUDE "/usr/include")
SET (ZLIB_INCLUDE "/usr/include")
SET (ZLIB_ARCHIVE "/usr/lib/libz.dylib")
SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_static.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
ENDIF (${DEVSIM_CONFIG} STREQUAL "osx_10.6.8")


IF (APPLE)
SET (CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation")
# don't use flat namespace, otherwise people can change out dynamic libraries
#SET (CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation -flat_namespace")
ADD_DEFINITIONS(-DTCL_THREADS -fvisibility=hidden)
ENDIF (APPLE)


ADD_DEFINITIONS(-DDEVSIM_VERSION_STRING="\\"Beta 0.01\\"")
ADD_DEFINITIONS(-DDEVSIM_COPYRIGHT_YEAR="\\"2009-2013\\"")

SET (SUBDIRS
    src
    testing
)

# superlu stuff
SITE_NAME(HOSTNAME)
MESSAGE("Hostname: ${HOSTNAME}")

ENABLE_TESTING()
FOREACH (SUB ${SUBDIRS})
    ADD_SUBDIRECTORY(${SUB})
ENDFOREACH (SUB ${SUBDIRS})

