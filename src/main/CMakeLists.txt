INCLUDE_DIRECTORIES (
    ../utility
    ../Geometry
    ../models
    ../math
    ../AutoEquation
    ../GeomModels
    ../Equation
    ../tclapi
    ../errorSystem
    ../Circuit/data
    ../Data
    ../meshing
    ../MathEval
    ../common_api
    ../pythonapi
    ${TCL_INCLUDE}
    ${PYTHON_INCLUDE}
)

SET (LIBRARIES
    tclapi_interpreter
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
    myThread
    circuitSources
    circuitIdeal
    circuitData
    errorSystem
    tclapi_api
    utility
)

SET (PYLIBRARIES
    commands
    pythonapi_interpreter
    commands
    pythonapi_interpreter
    Data
    AutoEquation
    meshing
    GeomModels
    Equation
    Geometry
    math
    MathEval
    models
    myThread
    circuitSources
    circuitIdeal
    circuitData
    errorSystem
    pythonapi_api
    utility
)

SET (OPTIONAL_LIBS "")
IF (VTKWRITER)
SET (OPTIONAL_LIBS ${OPTIONAL_LIBS} ${ZLIB_ARCHIVE})
ENDIF (VTKWRITER)
IF (GENIUSREADER)
SET (OPTIONAL_LIBS ${OPTIONAL_LIBS} ${CGNS_ARCHIVE})
ENDIF (GENIUSREADER)


IF (TCLMAIN)
ADD_EXECUTABLE (devsim_tcl devsim_tcl.cc)
TARGET_LINK_LIBRARIES(devsim_tcl ${LIBRARIES} ${TCL_ARCHIVE} ${SUPERLU_ARCHIVE} ${BLAS_ARCHIVE} ${SQLITE3_ARCHIVE} ${SYMDIFF_ARCHIVE} ${OPTIONAL_LIBS} ${PTHREAD_LIB} ${DLOPEN_LIB})
IF(APPLE)
SET_TARGET_PROPERTIES(devsim_tcl PROPERTIES LINK_FLAGS " -flat_namespace")
ENDIF(APPLE)
ENDIF (TCLMAIN)

ADD_EXECUTABLE (devsim_py devsim_py.cc)
TARGET_LINK_LIBRARIES(devsim_py ${PYLIBRARIES} ${PYTHON_ARCHIVE} ${SUPERLU_ARCHIVE} ${BLAS_ARCHIVE} ${SQLITE3_ARCHIVE} ${SYMDIFF_ARCHIVE} ${OPTIONAL_LIBS} ${PTHREAD_LIB} ${DLOPEN_LIB})
IF(APPLE)
SET_TARGET_PROPERTIES(devsim_py PROPERTIES LINK_FLAGS " -flat_namespace")
ENDIF(APPLE)

#IF (WIN32)
#SET_TARGET_PROPERTIES(devsim_tcl PROPERTIES
#        LINK_FLAGS " /NODEFAULTLIB:LIBMSVCRT"
#        )
#ENDIF (WIN32)


