ADD_DEFINITIONS(-fvisibility=hidden)
ADD_COMPILE_OPTIONS(-Wall)

SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)

SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)

SET (QUADMATH_ARCHIVE "-lquadmath")

# to prevent linking against bad libstdc++

SET (BLAS_ARCHIVE
 ${EXTERNAL_LIB}/getrf/build/libgetrf.a -Wl,--no-as-needed -lpthread -lblas -lm -ldl
)

SET (SUPERLU_INCLUDE /usr/include/SuperLU)
SET (SUPERLU_ARCHIVE  -lsuperlu)

SET (SQLITE3_ARCHIVE -lsqlite3)

SET (ZLIB_ARCHIVE -lz)

SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_dynamic.a)

# important flag for dynamic linking of static archives on linux
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

