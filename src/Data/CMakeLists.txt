SET (CXX_SRCS
    GlobalData.cc
)

INCLUDE_DIRECTORIES (
     ../utility
     ../Circuit
     ../Geometry
     ../meshing
     ../math
     ../errorSystem
     ../common_api
)

ADD_LIBRARY (Data STATIC ${CXX_SRCS})
