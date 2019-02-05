cmake_minimum_required(VERSION 3.12)
project(StudienarbeitCode)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")

add_executable(StudienarbeitCode src/main.cpp src/main.h src/DataPointFloat.cpp src/DataPointFloat.h src/RtreeNode.cpp src/RtreeNode.h src/Rtree.cpp src/Rtree.h)