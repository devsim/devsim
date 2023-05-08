ADD_COMPILE_OPTIONS(-Wall -fvisibility=hidden)

SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)

SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)

SET (QUADMATH_ARCHIVE "-lquadmath")

SET (BLAS_ARCHIVE -Wl,--no-as-needed -lpthread -lblas -lm -ldl)
SET (EXTENDED_BLAS_ARCHIVE ${EXTERNAL_LIB}/getrf/build/libgetrf.a)

SET (SQLITE3_ARCHIVE -lsqlite3)

SET (ZLIB_ARCHIVE -lz)

SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_dynamic.a)

# important flag for dynamic linking of static archives on linux
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

