cmake_minimum_required(VERSION 3.17)
project(bamf)

option(BAMF_BUILD_TESTS "Build tests" OFF)
option(BAMF_BUILD_TOOLS "Build tools" OFF)

find_package(fmt REQUIRED)
add_subdirectory(src)

if(BAMF_BUILD_TESTS)
    find_package(GTest REQUIRED)
    include(GoogleTest)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BAMF_BUILD_TOOLS)
    add_subdirectory(tools)
endif()
