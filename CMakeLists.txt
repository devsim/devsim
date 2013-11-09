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

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
ADD_DEFINITIONS(-DTCL_THREADS -pthread -fvisibility=hidden)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF (APPLE)
SET (CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation")
# don't use flat namespace, otherwise people can change out dynamic libraries
#SET (CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation -flat_namespace")
ADD_DEFINITIONS(-DTCL_THREADS -fvisibility=hidden)
ENDIF (APPLE)

# Ubuntu 12.04 32/64 bit
IF (${DEVSIM_CONFIG} STREQUAL "ubuntu_12.04")
ADD_DEFINITIONS(-std=c++0x)
SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)
SET (PYTHON_INCLUDE /usr/include/python2.7)
SET (PYTHON_ARCHIVE /usr/lib/libpython2.7.so)
# use /opt/acml5.3.0/util/cpuid.exe to find support
# ACML 5.3 available at developer.amd.com
SET (ZLIB_INCLUDE /usr/include)
IF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
SET (BLAS_ARCHIVE /opt/acml4.4.0/gfortran32/lib/libacml.a /usr/lib/gcc/i686-linux-gnu/4.6/libgfortran.a)
SET (ZLIB_ARCHIVE /usr/lib/i386-linux-gnu/libz.a)
SET (SQLITE3_ARCHIVE /usr/lib/i386-linux-gnu/libsqlite3.a)
ELSE (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
SET (BLAS_ARCHIVE /opt/acml5.3.0/gfortran64/lib/libacml.a /usr/lib/gcc/x86_64-linux-gnu/4.6/libgfortran.a)
SET (ZLIB_ARCHIVE /usr/lib/x86_64-linux-gnu/libz.a)
SET (SQLITE3_ARCHIVE /usr/lib/x86_64-linux-gnu/libsqlite3.a)
ENDIF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)


SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3.a)
SET (TCL_INCLUDE "/usr/include/tcl8.5")
SET (TCL_ARCHIVE "/usr/lib/libtcl8.5.a" -ldl)
SET (TCL_STUB /usr/lib/libtclstub8.5.a)
SET (PTHREAD_LIB -lpthread)
SET (SQLITE3_INCLUDE /usr/include)

SET (CGNS_ARCHIVE ${EXTERNAL_LIB}/cgnslib/lib/libcgns.a)
SET (CGNS_INCLUDE ${EXTERNAL_LIB}/cgnslib/include)


SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_static.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
ENDIF (${DEVSIM_CONFIG} STREQUAL "ubuntu_12.04")

# Redhat 6.4 64 bit (not tested on 32 bit)
IF (${DEVSIM_CONFIG} STREQUAL "redhat_6.4")
ADD_DEFINITIONS(-std=c++0x)
SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)
SET (PYTHON_INCLUDE /usr/include/python2.6)
# use /opt/acml5.3.0/util/cpuid.exe to find support
# ACML 5.3 available at developer.amd.com
SET (ZLIB_INCLUDE /usr/include)
IF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
# please put the correct path for libgfortran, if you know it
SET (BLAS_ARCHIVE /opt/acml4.4.0/gfortran32/lib/libacml.a /usr/lib/gcc/i686-linux-gnu/4.6/libgfortran.a)
SET (ZLIB_ARCHIVE /usr/lib/libz.so)
SET (SQLITE3_ARCHIVE /usr/lib/libsqlite3.so)
SET (TCL_STUB /usr/lib64/libtclstub8.5.a)
SET (TCL_ARCHIVE "/usr/lib/libtcl8.5.so" -ldl)
SET (PYTHON_ARCHIVE /usr/lib/libpython2.6.so)
ELSE (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
SET (BLAS_ARCHIVE /opt/acml5.3.0/gfortran64/lib/libacml.a /usr/lib/gcc/x86_64-redhat-linux/4.4.4/libgfortran.a)
SET (ZLIB_ARCHIVE /usr/lib64/libz.so)
SET (SQLITE3_ARCHIVE /usr/lib64/libsqlite3.so)
SET (TCL_STUB /usr/lib64/libtclstub8.5.a)
SET (TCL_ARCHIVE "/usr/lib64/libtcl8.5.so" -ldl)
SET (PYTHON_ARCHIVE /usr/lib64/libpython2.6.so)
ENDIF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)


SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3.a)
SET (TCL_INCLUDE "/usr/include/tcl8.5")
SET (PTHREAD_LIB -lpthread)
SET (SQLITE3_INCLUDE /usr/include)

SET (CGNS_ARCHIVE ${EXTERNAL_LIB}/cgnslib/lib/libcgns.a)
SET (CGNS_INCLUDE ${EXTERNAL_LIB}/cgnslib/include)

SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_static.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
ENDIF (${DEVSIM_CONFIG} STREQUAL "redhat_6.4")


IF (${DEVSIM_CONFIG} STREQUAL "osx_10.9")
SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)
SET (PYTHON_ARCHIVE "/usr/lib/libpython2.7.dylib")
SET (PYTHON_INCLUDE "/usr/include/python2.7")

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
SET (CGNS_ARCHIVE ${EXTERNAL_LIB}/cgnslib/lib/libcgns.a)
SET (CGNS_INCLUDE ${EXTERNAL_LIB}/cgnslib/include)
SET (ZLIB_INCLUDE "/usr/include")
SET (ZLIB_ARCHIVE "/usr/lib/libz.dylib")
SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_static.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
ENDIF (${DEVSIM_CONFIG} STREQUAL "osx_10.9")




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

