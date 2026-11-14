SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)

SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)

SET (BOOST_INCLUDE ${EXTERNAL_LIB}/boostorg/config/include ${EXTERNAL_LIB}/boostorg/math/include ${EXTERNAL_LIB}/boostorg/multiprecision/include)
ADD_DEFINITIONS(-DBOOST_MP_STANDALONE -DBOOST_MATH_STANDALONE)

SET (QUADMATH_ARCHIVE "-lquadmath")

SET (EXTENDED_BLAS_ARCHIVE ${CMAKE_BINARY_DIR}/../getrf/libgetrf.a)

SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
SET (SYMDIFF_ARCHIVE ${CMAKE_BINARY_DIR}/../symdiff/src/engine/libsymdiff_dynamic.a)

ADD_COMPILE_OPTIONS(-Wall -fvisibility=hidden)
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)
