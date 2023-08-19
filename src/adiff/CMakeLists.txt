INCLUDE_DIRECTORIES (
    ../errorSystem
)

SET (MC_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/mcParser.cc
    ${CMAKE_CURRENT_BINARY_DIR}/mcScanner.cc
    mcModelCompiler.cc
    mcTest.cc
    mcModel.cc
)

SET (CXX_SRCS
    EquationObject.cc
    UserFunc.cc
    LogicalFunc.cc
)

SET (MC_CXX_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/mcParser.cc
    ${CMAKE_CURRENT_BINARY_DIR}/mcScanner.cc
)

IF (WIN32)
SET_SOURCE_FILES_PROPERTIES(mcScanner.cc mcParser.cc PROPERTIES GENERATED TRUE)
ELSE (WIN32)
SET_SOURCE_FILES_PROPERTIES(mcScanner.cc mcParser.cc PROPERTIES GENERATED TRUE COMPILE_FLAGS "-Wno-unused-function -Wno-sign-compare")
ENDIF (WIN32)


INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../utility
)

#ADD_LIBRARY (adiff STATIC ${DS_SRCS} ${CXX_SRCS})
ADD_EXECUTABLE (modelcomp ${MC_SRCS} ${CXX_SRCS})

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mcScanner.cc
    COMMAND ${FLEX}
    ARGS    -Pmc -omcScanner.cc ${CMAKE_CURRENT_SOURCE_DIR}/mcScanner.l
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/mcParser.hh ${CMAKE_CURRENT_SOURCE_DIR}/mcScanner.l
		    test.hh EquationObject.hh UserFunc.hh
)

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mcParser.cc ${CMAKE_CURRENT_BINARY_DIR}/mcParser.hh
    COMMAND ${BISON}
    ARGS -pmc --debug -v -d -o mcParser.cc ${CMAKE_CURRENT_SOURCE_DIR}/mcParser.y
    # these must be read from parser.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mcParser.y EquationObject.hh UserFunc.hh test.hh mcModelCompiler.hh mcModel.hh
)

