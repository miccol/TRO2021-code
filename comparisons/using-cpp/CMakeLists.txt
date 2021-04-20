cmake_minimum_required(VERSION 3.5)

project(tro-barriers LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-lgtest -std=c++11  -lpthread)

find_package (Threads)

add_executable(barriers-cv main.cpp barrier.cpp)
target_link_libraries (barriers-cv ${CMAKE_THREAD_LIBS_INIT})
