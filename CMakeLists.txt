cmake_minimum_required(VERSION 3.8)
project(intrusive LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter")
set(CMAKE_CXX_EXTENSIONS Off)

enable_testing()
add_subdirectory(test)

