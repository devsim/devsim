ADD_COMPILE_OPTIONS(-Wall -fvisibility=hidden)

SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)
SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
SET (ZLIB_INCLUDE /usr/include)

SET (BLAS_ARCHIVE /usr/lib/x86_64-linux-gnu/liblapack.so.3 /usr/lib/x86_64-linux-gnu/libblas.so.3 ${EXTERNAL_LIB}/getrf/build/libgetrf.a)
SET (ZLIB_ARCHIVE -lz )
SET (SQLITE3_ARCHIVE -lsqlite3)


SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3.a)
SET (BOOST_INCLUDE "/usr/include")
SET (QUADMATH_ARCHIVE "-lquadmath")
SET (DLOPEN_LIB -ldl)
SET (SQLITE3_INCLUDE /usr/include)

SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_dynamic.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)

# important flag for dynamic linking of static archives on linux
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

