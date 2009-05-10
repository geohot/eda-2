cmake_minimum_required(VERSION 2.6)

#to get sources if added
#find -name "*.cc"

PROJECT(eda-reversing)
SET(EDA_SRCS
./Address.cc
./Changelist.cc
#./InstructionFactoryARM.cc
./Memory.cc
)
#INCLUDE_DIRECTORIES(./include/)
ADD_LIBRARY(eda_lib ${EDA_SRCS})
ADD_EXECUTABLE(eda main.cc)
TARGET_LINK_LIBRARIES(eda eda_lib pthread)

