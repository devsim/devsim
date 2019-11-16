# Fedora release 27 (Twenty Seven) Only 64 bits
ADD_DEFINITIONS(-pthread -fvisibility=hidden)
SET (BISON /usr/bin/bison)
SET (DLOPEN_LIB -ldl)
SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
SET (FLEX /usr/bin/flex)
SET (PTHREAD_LIB -lpthread)
SET (QUADMATH_ARCHIVE "-lquadmath")

# Python
SET (PYTHON27_INCLUDE /usr/include/python2.7)
SET (PYTHON27_ARCHIVE -lpython2.7)
SET (PYTHON36_INCLUDE /usr/include/python3.6m)
SET (PYTHON36_ARCHIVE -lpython3.6m)

# Superlu
SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3.a)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)

# Symdiff
SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_dynamic.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)

# Zlib
SET (ZLIB_ARCHIVE -lz )
SET (ZLIB_INCLUDE /usr/include)

# Sqlite3
SET (SQLITE3_ARCHIVE -lsqlite3)
SET (SQLITE3_INCLUDE "/usr/include")

# Blas & Lapack
SET (BLAS_ARCHIVE -llapack -lblas ${EXTERNAL_LIB}/getrf/build/libgetrf.a)
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

