# set min version of cmake required
cmake_minimum_required(VERSION 3.0)

# set bin dir
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -lpthread")

# set project name
project(raytracer)

# set sources
file(GLOB SOURCES "src/*.cpp")

# add compile options
add_compile_options(-Wall -Wextra -Wpedantic)

# add exe
add_executable(raytracer ${SOURCES})
