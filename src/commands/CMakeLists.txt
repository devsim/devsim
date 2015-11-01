SET (CXX_SRCS
    GeometryCommands.cc
    MaterialCommands.cc
    ModelCommands.cc
    EquationCommands.cc
    MathCommands.cc
    MeshingCommands.cc
    CircuitCommands.cc
    Validate.cc
    CheckFunctions.cc
)

IF (VTKWRITER)
ADD_DEFINITIONS(-DVTKWRITER)
ENDIF (VTKWRITER)
IF (GENIUSREADER)
ADD_DEFINITIONS(-DGENIUSREADER)
ENDIF (GENIUSREADER)

INCLUDE_DIRECTORIES (
    ../commands
    ../common_api
    ../Geometry
    ../models
    ../math
    ../utility
    ../errorSystem
    ../AutoEquation
    ../Equation
    ../meshing
    ../Data
    ../Circuit/data
    ../Circuit/models/sources
    ../Circuit/models/ideal
    ../GeomModels
    ../MathEval
)


ADD_LIBRARY (commands ${CXX_SRCS})


