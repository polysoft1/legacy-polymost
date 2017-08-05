cmake_minimum_required(VERSION 3.0)

project(Polymost)

include_directories(${PROJECT_BINARY_DIR})

add_executable(PolyMost plugin.cpp)

# define the project
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

