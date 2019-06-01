cmake_minimum_required(VERSION 3.11)
project(StudienarbeitCode)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG -fopenmp")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -march=skylake-avx512 -fopenmp")

add_executable(StudienarbeitCode src/main.cpp src/main.h src/DataPointFloat.cpp src/DataPointFloat.h src/RtreeNode.cpp src/RtreeNode.h src/Rtree.cpp src/Rtree.h src/DBSCAN.cpp src/DBSCAN.h)