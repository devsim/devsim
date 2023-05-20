SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)
SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
SET (EIGEN_INCLUDE ${EXTERNAL_LIB}/eigen)
SET (ZLIB_INCLUDE /usr/include)

SET (BLAS_ARCHIVE /usr/lib/x86_64-linux-gnu/liblapack.so.3 /usr/lib/x86_64-linux-gnu/libblas.so.3)
SET (EXTENDED_BLAS_ARCHIVE ${EXTERNAL_LIB}/getrf/build/libgetrf.a)
SET (ZLIB_ARCHIVE -lz )
SET (SQLITE3_ARCHIVE -lsqlite3)


SET (BOOST_INCLUDE ${EXTERNAL_LIB}/boostorg/config/include ${EXTERNAL_LIB}/boostorg/math/include ${EXTERNAL_LIB}/boostorg/multiprecision/include)
ADD_DEFINITIONS(-DBOOST_MP_STANDALONE -DBOOST_MATH_STANDALONE)
SET (QUADMATH_ARCHIVE "-lquadmath")
SET (DLOPEN_LIB -ldl)
SET (SQLITE3_INCLUDE /usr/include)

SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_dynamic.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)

ADD_COMPILE_OPTIONS(-Wall -fvisibility=hidden)
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

