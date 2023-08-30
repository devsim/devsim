SET (CXX_SRCS
    GlobalData.cc
)

IF (DEVSIM_MATERIALDB)
SET (CXX_SRCS ${CXX_SRCS} MaterialDBEntry.cc MaterialDB.cc)
ENDIF ()

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

ADD_LIBRARY (Data STATIC ${CXX_SRCS})
