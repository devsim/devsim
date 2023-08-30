SET (CXX_SRCS
    CircuitNode.cc
    InstanceKeeper.cc
    InstanceModel.cc
    NodeKeeper.cc
)

INCLUDE_DIRECTORIES (
    ../../utility
    ../models/sources
    ../../Data
    ../../errorSystem
    ../../common_api
)

ADD_LIBRARY (circuitData STATIC ${CXX_SRCS})
