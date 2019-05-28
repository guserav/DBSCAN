cmake_minimum_required(VERSION 3.11)
project(StudienarbeitCode)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -flto")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -march=skylake-avx512 -flto")

add_executable(StudienarbeitCode src/all.cpp)