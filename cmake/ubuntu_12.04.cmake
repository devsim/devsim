# Ubuntu 12.04 32/64 bit
#IF (${DEVSIM_CONFIG} STREQUAL "ubuntu_12.04")
ADD_DEFINITIONS(-DTCL_THREADS -pthread -fvisibility=hidden)
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
#ENDIF (${DEVSIM_CONFIG} STREQUAL "ubuntu_12.04")
