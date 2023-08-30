SET (SRCS
    IdealCapacitor
    IdealInductor
    IdealResistor
)

SET (CXX_SRCS
    IdealCapacitor.cc
    IdealInductor.cc
    IdealResistor.cc
)

#### SKIP generation of models on WIN32 until this is well behaved.
IF (WIN32)
ELSEIF (APPLE)
ELSEIF (CMAKE_CROSSCOMPILING)
ELSE (WIN32)
FOREACH (I ${SRCS})
SET_SOURCE_FILES_PROPERTIES(${I}.cc PROPERTIES GENERATED TRUE)
SET_SOURCE_FILES_PROPERTIES(${I}.hh PROPERTIES GENERATED TRUE)
#SET (CXX_SRCS "${CXX_SRCS} ${I}.cc")

ADD_CUSTOM_COMMAND(
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${I}.cc ${CMAKE_CURRENT_SOURCE_DIR}/${I}.hh
    COMMAND ${PROJECT_BINARY_DIR}/adiff/modelcomp
    ARGS < ${CMAKE_CURRENT_SOURCE_DIR}/${I}.mc > ${CMAKE_CURRENT_SOURCE_DIR}/${I}.out 2>&1
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${I}.mc ${PROJECT_BINARY_DIR}/adiff/modelcomp
)
ENDFOREACH (I ${SRCS})
ENDIF (WIN32)

INCLUDE_DIRECTORIES (
    ../../data
    ../../../math
    ../../../utility
)

#ADD_SUBDIRECTORY(data)
ADD_LIBRARY (circuitIdeal STATIC ${CXX_SRCS})
