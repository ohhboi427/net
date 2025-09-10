cmake_minimum_required(VERSION 3.21..4.0)

project(Net
    LANGUAGES CXX
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

include(FetchContent)

add_subdirectory("src")
add_subdirectory("example")
