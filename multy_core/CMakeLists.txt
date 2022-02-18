cmake_minimum_required(VERSION 3.20)
project(multy_core)

set(CMAKE_CXX_STANDARD 23)
set (CMAKE_EXE_LINKER_FLAGS -pthread )

add_executable(multy_core main.cpp)
