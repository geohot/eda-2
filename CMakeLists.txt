cmake_minimum_required(VERSION 2.6)
INCLUDE (CheckLibraryExists)

#to get sources if added
#find -name "*.cc"

PROJECT(eda-reversing)
SET(EDA_SRCS
./Address.cc
./Changelist.cc
./ChangelistFactory.cc
./StatelessChangelist.cc
./InstructionFactory.cc
./InstructionFactoryARM.cc
./Memory.cc
./util.cc
./debug.cc
./History.cc
./ParsedInstruction.cc
./Servlet.cc
./FactoryOwner.cc
./File.cc
./Instruction.cc
)
#INCLUDE_DIRECTORIES(./include/)
ADD_LIBRARY(eda_lib ${EDA_SRCS})
ADD_EXECUTABLE(eda main.cc)

SET(EXTRALIBS "")
CHECK_LIBRARY_EXISTS(wsock32 WSAStartup "" HAVE_LIBWSOCK32)
IF(HAVE_LIBWSOCK32)
  SET(EXTRALIBS ${EXTRALIBS} "wsock32")
ENDIF(HAVE_LIBWSOCK32)

TARGET_LINK_LIBRARIES(eda eda_lib ${EXTRALIBS})
