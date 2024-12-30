cmake_minimum_required(VERSION 3.22)

project(PhysicalModellingFan)

set(CMAKE_CXX_STANDARD 23)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

include(cmake/cpm.cmake)

enable_testing()

CPMAddPackage(
    NAME JUCE
    GITHUB_REPOSITORY juce-framework/JUCE
    GIT_TAG 8.0.4
    VERSION 8.0.4
    SOURCE_DIR ${LIB_DIR}/juce
)

CPMAddPackage(
    NAME GOOGLETEST
    GITHUB_REPOSITORY google/googletest
    VERSION 1.15.2
    SOURCE_DIR ${LIB_DIR}/googletest
    OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"
)

add_subdirectory(plugin)
add_subdirectory(test)