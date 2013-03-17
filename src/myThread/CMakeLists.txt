SET (CXX_SRCS
  myqueue.cc
  myworker.cc
  mypacket.cc
  myThreadPool.cc
)

INCLUDE_DIRECTORIES (
  ../utility
  ../Data
  ../errorSystem
  ../common_api
)

ADD_LIBRARY (myThread ${CXX_SRCS})

