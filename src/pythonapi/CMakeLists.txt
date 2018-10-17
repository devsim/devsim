SET (CXX_SRCS1
   GetArgs.cc
   PythonAppInit.cc
   PythonCommands.cc
   CommandHandler.cc
)

SET (CXX_SRCS2
    ObjectHolder.cc
    OutputStream.cc
    Interpreter.cc
    dsTimer.cc
    mycondition.cc
    mymutex.cc
    threadBaseClass.cc
)

INCLUDE_DIRECTORIES (
    ../utility
    ../pythonapi
    ../commands
    ../common_api
)

ADD_LIBRARY (pythonapi_interpreter OBJECT ${CXX_SRCS1})
ADD_LIBRARY (pythonapi_api OBJECT ${CXX_SRCS2})
TARGET_INCLUDE_DIRECTORIES (pythonapi_interpreter PUBLIC ${PYTHON_INCLUDE})
TARGET_INCLUDE_DIRECTORIES (pythonapi_api PUBLIC ${PYTHON_INCLUDE})
SET_PROPERTY(TARGET pythonapi_interpreter PROPERTY POSITION_INDEPENDENT_CODE ON)
SET_PROPERTY(TARGET pythonapi_api PROPERTY POSITION_INDEPENDENT_CODE ON)
#SET_TARGET_PROPERTIES(pythonapi_interpreter PROPERTIES LINK_FLAGS " -flat_namespace")
#SET_TARGET_PROPERTIES(pythonapi_api PROPERTIES LINK_FLAGS " -flat_namespace")



IF (PYTHON3)
ADD_LIBRARY (python3api_interpreter OBJECT ${CXX_SRCS1})
ADD_LIBRARY (python3api_api OBJECT ${CXX_SRCS2})
TARGET_INCLUDE_DIRECTORIES (python3api_interpreter BEFORE PUBLIC ${PYTHON3_INCLUDE})
TARGET_INCLUDE_DIRECTORIES (python3api_api BEFORE PUBLIC ${PYTHON3_INCLUDE})
SET_PROPERTY(TARGET python3api_interpreter PROPERTY POSITION_INDEPENDENT_CODE ON)
SET_PROPERTY(TARGET python3api_api PROPERTY POSITION_INDEPENDENT_CODE ON)
#SET_TARGET_PROPERTIES(python3api_interpreter PROPERTIES LINK_FLAGS " -flat_namespace")
#SET_TARGET_PROPERTIES(python3api_api PROPERTIES LINK_FLAGS " -flat_namespace")
ENDIF (PYTHON3)

