cmake_minimum_required(VERSION 3.15)
SET (CMAKE_C_COMPILER "gcc")
SET (CMAKE_CXX_COMPILER "g++")
project(app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mavx -O3")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_LIST_DIR}/cmake")
set(DEPS_PREFIX "_deps")

aux_source_directory (src SRC_LIST)
include_directories (src/include)

add_executable(app ${SRC_LIST})
