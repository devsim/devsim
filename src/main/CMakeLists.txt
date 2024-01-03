INCLUDE_DIRECTORIES (
    ../utility
    ../AutoEquation
    ../Data
    ../common_api
    ../math
    ../Circuit/data
    ../meshing
    ../MathEval
    ${SYMDIFF_INCLUDE}
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
IF (DEVSIM_EXTENDED_PRECISION)
SET (OPTIONAL_LIBS ${OPTIONAL_LIBS} ${QUADMATH_ARCHIVE} ${BOOST_ARCHIVE})
ENDIF (DEVSIM_EXTENDED_PRECISION)


IF (PYTHON3)
ADD_LIBRARY (devsim_static_py3 STATIC devsim_py.cc ResetAllData.cc $<TARGET_OBJECTS:pythonapi_interpreter_py3>)
IF (NOT STATICONLY)
ADD_LIBRARY (devsim_py3 SHARED devsim_py.cc ResetAllData.cc $<TARGET_OBJECTS:pythonapi_interpreter_py3>)

IF(WIN32 OR PYTHON3_ARCHIVE)
TARGET_LINK_LIBRARIES(devsim_py3 ${PY3LIBRARIES} ${PYTHON3_ARCHIVE} ${SUPERLU_ARCHIVE} ${EXTENDED_BLAS_ARCHIVE} ${SQLITE3_ARCHIVE} ${SYMDIFF_ARCHIVE} ${OPTIONAL_LIBS} ${DLOPEN_LIB})
ELSE()
# macOS and Linux do not need to dynamically link against python
# this makes the dynamic library more portable
TARGET_LINK_LIBRARIES(devsim_py3 ${PY3LIBRARIES} ${SUPERLU_ARCHIVE} ${EXTENDED_BLAS_ARCHIVE} ${SQLITE3_ARCHIVE} ${SYMDIFF_ARCHIVE} ${OPTIONAL_LIBS} ${DLOPEN_LIB})
ENDIF()
IF (APPLE)
SET_TARGET_PROPERTIES(devsim_py3 PROPERTIES
  LINK_FLAGS "-undefined dynamic_lookup -flat_namespace"
  INSTALL_RPATH "@loader_path;@loader_path/../lib;@loader_path/../../../../lib;@executable_path/../lib"
  BUILD_WITH_INSTALL_RPATH TRUE
)
ENDIF (APPLE)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
SET_TARGET_PROPERTIES(devsim_py3 PROPERTIES
  # the last bit of the INSTALL_RPATH to match ./lib and ./lib/python3.x/site-packages/devsim
  INSTALL_RPATH "\$ORIGIN:\$ORIGIN/../lib:\$ORIGIN/../../../../lib"
  BUILD_WITH_INSTALL_RPATH TRUE
)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(WIN32)
SET_TARGET_PROPERTIES(devsim_py3  PROPERTIES PREFIX "" SUFFIX ".pyd" OUTPUT_NAME  "devsim_py3")
ELSE(WIN32)
SET_TARGET_PROPERTIES(devsim_py3  PROPERTIES PREFIX "" SUFFIX ".so" OUTPUT_NAME "devsim_py3")
ENDIF(WIN32)
ENDIF(NOT STATICONLY)
ENDIF (PYTHON3)


