SET (CXX_SRCS
    base64.cc
    FPECheck.cc
    dsAssert.cc
    dsException.cc
    GetGlobalParameter.cc
)

INCLUDE_DIRECTORIES (
    ../errorSystem
    ../Data
    ../common_api
    ${ZLIB_INCLUDE}
    ${OPENSSL_INCLUDE}
)

ADD_LIBRARY (utility ${CXX_SRCS})
#ADD_EXECUTABLE (test_base64 test_base64.cc)
#TARGET_LINK_LIBRARIES(test_base64 utility ${ZLIB_ARCHIVE})

