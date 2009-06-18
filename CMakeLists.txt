cmake_minimum_required(VERSION 2.6)

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
./InstructionFactoryISDF.cc
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
IF(WIN32)
  SET(EXTRALIBS ${EXTRALIBS} "wsock32")
ENDIF(WIN32)

TARGET_LINK_LIBRARIES(eda eda_lib ${EXTRALIBS})
