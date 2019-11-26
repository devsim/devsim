INCLUDE_DIRECTORIES (
    ../utility
    ../Geometry
    ../models
    ../math
    ../AutoEquation
    ../GeomModels
    ../Equation
    ../errorSystem
    ../Circuit/data
    ../Data
    ../meshing
    ../MathEval
    ../common_api
    ../pythonapi
)

SET (PY3LIBRARIES
    commands
    Data
    AutoEquation
    meshing
    GeomModels
    Equation
    Geometry
    math
    MathEval
    models
    circuitSources
    circuitIdeal
    circuitData
    errorSystem
    utility
)

SET (OPTIONAL_LIBS "")
IF (VTKWRITER)
SET (OPTIONAL_LIBS ${OPTIONAL_LIBS} ${ZLIB_ARCHIVE})
ENDIF (VTKWRITER)
IF (GENIUSREADER)
SET (OPTIONAL_LIBS ${OPTIONAL_LIBS} ${CGNS_ARCHIVE})
ENDIF (GENIUSREADER)
IF (DEVSIM_EXTENDED_PRECISION)
SET (OPTIONAL_LIBS ${OPTIONAL_LIBS} ${QUADMATH_ARCHIVE} ${BOOST_ARCHIVE})
ENDIF (DEVSIM_EXTENDED_PRECISION)


IF (PYTHON3)
ADD_LIBRARY (devsim_py37 SHARED devsim_py.cc $<TARGET_OBJECTS:pythonapi_interpreter_py37>)
IF(WIN32)
TARGET_LINK_LIBRARIES(devsim_py37 ${PY3LIBRARIES} ${PYTHON3_ARCHIVE} ${SUPERLU_ARCHIVE} ${BLAS_ARCHIVE} ${SQLITE3_ARCHIVE} ${SYMDIFF_ARCHIVE} ${OPTIONAL_LIBS} ${PTHREAD_LIB} ${DLOPEN_LIB})
ELSE(WIN32)
# macOS and Linux do not need to dynamically link against python
# this makes the dynamic library more portable
TARGET_LINK_LIBRARIES(devsim_py37 ${PY3LIBRARIES} ${SUPERLU_ARCHIVE} ${BLAS_ARCHIVE} ${SQLITE3_ARCHIVE} ${SYMDIFF_ARCHIVE} ${OPTIONAL_LIBS} ${PTHREAD_LIB} ${DLOPEN_LIB})
ENDIF(WIN32)
IF (APPLE)
SET_TARGET_PROPERTIES(devsim_py37 PROPERTIES LINK_FLAGS "-undefined dynamic_lookup -flat_namespace")
ENDIF (APPLE)
ENDIF (PYTHON3)

IF (PYTHON3)
IF(WIN32)
SET_TARGET_PROPERTIES(devsim_py37  PROPERTIES SUFFIX ".pyd" OUTPUT_NAME  "devsim_py3")
ELSE(WIN32)
SET_TARGET_PROPERTIES(devsim_py37  PROPERTIES PREFIX "" SUFFIX ".so" OUTPUT_NAME "devsim_py3")
ENDIF(WIN32)
ENDIF (PYTHON3)

