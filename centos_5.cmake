# Redhat 6.5 64 bit (not tested on 32 bit)
#IF (${DEVSIM_CONFIG} STREQUAL "redhat_6.5")
ADD_DEFINITIONS(-DTCL_THREADS -pthread -fvisibility=hidden)
SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)
SET (PYTHON_INCLUDE $ENV{HOME}/anaconda/include/python2.7)
# use /opt/acml5.3.0/util/cpuid.exe to find support
# ACML 5.3 available at developer.amd.com
SET (ZLIB_INCLUDE $ENV{HOME}/anaconda/include)
IF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
# please put the correct path for libgfortran, if you know it
SET (BLAS_ARCHIVE -llapack -lblas)
SET (ZLIB_ARCHIVE $ENV{HOME}/anaconda/lib/libz.a)
SET (SQLITE3_ARCHIVE $ENV{HOME}/anaconda/lib/libsqlite3.a)
SET (TCL_STUB $ENV{HOME}/lib/libtclstub8.5.a)
SET (TCL_ARCHIVE "$ENV{HOME}/libibtcl8.5.so" -ldl)
SET (PYTHON_ARCHIVE $ENV{HOME}/anaconda/lib/libpython2.7.so)
ELSE (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
SET (BLAS_ARCHIVE -llapack -lblas)
SET (ZLIB_ARCHIVE $ENV{HOME}/anaconda/lib/libz.a)
SET (SQLITE3_ARCHIVE $ENV{HOME}/anaconda/lib/libsqlite3.a)
SET (TCL_STUB $ENV{HOME}/anaconda/lib/libtclstub8.5.a)
SET (TCL_ARCHIVE "$ENV{HOME}/anaconda/lib/libtcl8.5.so" -ldl)
SET (PYTHON_ARCHIVE $ENV{HOME}/anaconda/lib/libpython2.7.so)
ENDIF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)


SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3.a)
SET (TCL_INCLUDE "$ENV{HOME}/anaconda/include")
SET (PTHREAD_LIB -lpthread)
SET (SQLITE3_INCLUDE $ENV{HOME}/anaconda/include)

SET (CGNS_ARCHIVE ${EXTERNAL_LIB}/cgnslib/lib/libcgns.a)
SET (CGNS_INCLUDE ${EXTERNAL_LIB}/cgnslib/include)

SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_static.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)

#ENDIF (${DEVSIM_CONFIG} STREQUAL "redhat_6.5")

