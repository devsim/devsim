SET (CXX_SRCS
    MeshLoaderStructs.cc
    MeshLoaderUtility.cc
    DevsimRestartWriter.cc
    DevsimReader.cc
    DevsimParser.cc
    DevsimScanner.cc
    DevsimLoader.cc
    DevsimWriter.cc
    GmshLoader.cc
    GmshReader.cc
    GmshParser.cc
    GmshScanner.cc
    MeshKeeper.cc
    Mesh.cc
    MeshWriter.cc
    FloodsWriter.cc
    MeshUtil.cc
    Mesh1d.cc
    Mesh1dStructs.cc
    Mesh2d.cc
    Mesh2dStructs.cc
    TecplotWriter.cc
)

IF (VTKWRITER)
SET (CXX_SRCS ${CXX_SRCS} VTKWriter.cc)
ENDIF (VTKWRITER)

INCLUDE_DIRECTORIES (
    ../utility
    ../Geometry
    ../GeomModels
    ../Data
    ../math
    ../meshing
    ../models
    ../errorSystem
    ../AutoEquation
    ../Equation
    ../common_api
)


IF (WIN32)
SET_SOURCE_FILES_PROPERTIES(DevsimScanner.cc DevsimParser.cc PROPERTIES GENERATED TRUE)
ELSE (WIN32)
SET_SOURCE_FILES_PROPERTIES(DevsimScanner.cc DevsimParser.cc PROPERTIES GENERATED TRUE COMPILE_FLAGS "-Wno-unused-function -Wno-sign-compare")
ENDIF(WIN32)

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/DevsimScanner.cc
    COMMAND ${FLEX}
    ARGS    -PDevsim -oDevsimScanner.cc ${CMAKE_CURRENT_SOURCE_DIR}/DevsimScanner.l
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/DevsimParser.hh ${CMAKE_CURRENT_SOURCE_DIR}/DevsimScanner.l
)

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/DevsimParser.cc ${CMAKE_CURRENT_BINARY_DIR}/DevsimParser.hh
    COMMAND ${BISON}
    ARGS -pDevsim --debug -v -d -o DevsimParser.cc ${CMAKE_CURRENT_SOURCE_DIR}/DevsimParser.y
    # these must be read from parser.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/DevsimParser.y
)

IF (WIN32)
SET_SOURCE_FILES_PROPERTIES(GmshScanner.cc GmshParser.cc PROPERTIES GENERATED TRUE)
ELSE (WIN32)
SET_SOURCE_FILES_PROPERTIES(GmshScanner.cc GmshParser.cc PROPERTIES GENERATED TRUE COMPILE_FLAGS "-Wno-unused-function -Wno-sign-compare")
ENDIF (WIN32)

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/GmshScanner.cc
    COMMAND ${FLEX}
    ARGS    -PGmsh -oGmshScanner.cc ${CMAKE_CURRENT_SOURCE_DIR}/GmshScanner.l
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/GmshParser.hh ${CMAKE_CURRENT_SOURCE_DIR}/GmshScanner.l
)

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/GmshParser.cc ${CMAKE_CURRENT_BINARY_DIR}/GmshParser.hh
    COMMAND ${BISON}
    ARGS -pGmsh --debug -v -d -o GmshParser.cc ${CMAKE_CURRENT_SOURCE_DIR}/GmshParser.y
    # these must be read from parser.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/GmshParser.y
)

ADD_LIBRARY (meshing STATIC ${CXX_SRCS})

