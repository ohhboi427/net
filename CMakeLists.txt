cmake_minimum_required(VERSION 3.21..4.0)

option(NET_BUILD_TESTS "Build the testing target" OFF)

project(Net
    LANGUAGES CXX
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

include(FetchContent)

add_subdirectory("src")
add_subdirectory("examples")

if(NET_BUILD_TESTS)
    enable_testing()
    add_subdirectory("tests")
endif()
