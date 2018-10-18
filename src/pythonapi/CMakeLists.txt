SET (CXX_SRCS1
   GetArgs.cc
   PythonAppInit.cc
   PythonCommands.cc
   CommandHandler.cc
   ObjectHolder.cc
   OutputStream.cc
   Interpreter.cc
)

SET (CXX_SRCS2
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
TARGET_INCLUDE_DIRECTORIES (pythonapi_interpreter BEFORE PRIVATE ${PYTHON_INCLUDE})
#SET_PROPERTY(TARGET pythonapi_interpreter PROPERTY POSITION_INDEPENDENT_CODE ON)
#SET_PROPERTY(TARGET pythonapi_api PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(pythonapi_interpreter PRIVATE -DDEVSIM_MODULE_NAME=devsim_py27)


IF (PYTHON3)
ADD_LIBRARY (python3api_interpreter OBJECT ${CXX_SRCS1})
ADD_LIBRARY (python3api_api OBJECT ${CXX_SRCS2})
TARGET_INCLUDE_DIRECTORIES (python3api_interpreter BEFORE PUBLIC ${PYTHON3_INCLUDE})
#TARGET_INCLUDE_DIRECTORIES (python3api_api BEFORE PUBLIC ${PYTHON3_INCLUDE})
#SET_PROPERTY(TARGET python3api_interpreter PROPERTY POSITION_INDEPENDENT_CODE ON)
#SET_PROPERTY(TARGET python3api_api PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(python3api_interpreter PRIVATE -DDEVSIM_MODULE_NAME=devsim_py36)
ENDIF (PYTHON3)

