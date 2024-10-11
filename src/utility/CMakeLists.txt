SET (CXX_SRCS
    FPECheck.cc
    dsAssert.cc
    dsException.cc
    GetGlobalParameter.cc
    GetNumberOfThreads.cc
    dsTimer.cc
    base64.cc
)

INCLUDE_DIRECTORIES (
    ../errorSystem
    ../Data
    ../common_api
    ../utility
)

ADD_LIBRARY (utility STATIC ${CXX_SRCS})

