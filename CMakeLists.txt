cmake_minimum_required(VERSION 3.10)
project(hibot)

set(PROJECT_NAME "log_collector")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -pedantic -fPIC -pipe")

include_directories(${CMAKE_SOURCE_DIR}/contrib/include)
include_directories(${CMAKE_SOURCE_DIR}/src/)

add_executable( ${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/src/tools/log_collector.cpp
)

target_link_libraries(${PROJECT_NAME}
    workflow
    rt
    gflags
    pthread
    ssl
    crypto
    yaml-cpp
)
