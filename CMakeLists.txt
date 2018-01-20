cmake_minimum_required(VERSION 3.0) # set min version of cmake required
project(raytracer)                  # set project name

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/) # set bin dir

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -lpthread")

# set sources
file(GLOB SOURCES "src/*.cpp")

# add compile options
add_compile_options(-Wall -Wextra -Wpedantic)

# include yaml-cpp
include_directories(${CMAKE_SOURCE_DIR}/middleware/yaml-cpp/include)
link_directories(${CMAKE_SOURCE_DIR}/middleware/yaml-cpp/lib)

add_executable(raytracer ${SOURCES})

target_link_libraries(raytracer yaml-cpp)
