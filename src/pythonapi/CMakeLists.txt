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


ADD_LIBRARY (pythonapi_interpreter ${CXX_SRCS1})
ADD_LIBRARY (pythonapi_api ${CXX_SRCS2})
TARGET_INCLUDE_DIRECTORIES (pythonapi_interpreter PUBLIC ${PYTHON_INCLUDE})
TARGET_INCLUDE_DIRECTORIES (pythonapi_api PUBLIC ${PYTHON_INCLUDE})

IF (PYTHON3)
ADD_LIBRARY (python3api_interpreter ${CXX_SRCS1})
ADD_LIBRARY (python3api_api ${CXX_SRCS2})
TARGET_INCLUDE_DIRECTORIES (python3api_interpreter PUBLIC ${PYTHON3_INCLUDE})
TARGET_INCLUDE_DIRECTORIES (python3api_api PUBLIC ${PYTHON3_INCLUDE})
ENDIF (PYTHON3)
