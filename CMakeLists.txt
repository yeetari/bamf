cmake_minimum_required(VERSION 3.17)
project(bamf)

option(BAMF_BUILD_EXAMPLE "Build example" OFF)

add_subdirectory(src)

if(BAMF_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
