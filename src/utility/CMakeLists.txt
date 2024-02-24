SET (CXX_SRCS
    FPECheck.cc
    dsAssert.cc
    dsException.cc
    GetGlobalParameter.cc
    GetNumberOfThreads.cc
    dsTimer.cc
)

IF (VTKWRITER)
SET (CXX_SRCS ${CXX_SRCS} base64.cc)
ENDIF (VTKWRITER)

INCLUDE_DIRECTORIES (
    ../errorSystem
    ../Data
    ../common_api
    ../utility
    ${ZLIB_INCLUDE}
)

ADD_LIBRARY (utility STATIC ${CXX_SRCS})

#ADD_EXECUTABLE (test_base64 test_base64.cc)
#TARGET_LINK_LIBRARIES(test_base64 utility ${ZLIB_ARCHIVE})


