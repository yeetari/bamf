cmake_minimum_required(VERSION 3.17)
project(bamf)

option(BAMF_BUILD_DRIVER "Build example" OFF)
option(BAMF_BUILD_TESTS "Build tests" OFF)

add_subdirectory(src)

if(BAMF_BUILD_DRIVER)
    add_subdirectory(driver)
endif()

if(BAMF_BUILD_TESTS)
    add_subdirectory(tests)
endif()
