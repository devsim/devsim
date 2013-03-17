SET (CXX_SRCS
    GlobalData.cc
    MaterialDBEntry.cc
    MaterialDB.cc
)

INCLUDE_DIRECTORIES (
     ../utility
     ../Circuit
     ../Geometry 
     ../meshing
     ../math
     ../errorSystem
     ../common_api
     ${SQLITE3_INCLUDE}
)

ADD_LIBRARY (Data ${CXX_SRCS})
