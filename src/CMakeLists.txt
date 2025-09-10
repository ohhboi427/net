set(NET_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(NET_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

file(GLOB_RECURSE NET_SOURCES CONFIGURE_DEPENDS
    "${NET_SOURCE_DIR}/*.cpp"
    "${NET_SOURCE_DIR}/*.hpp"
    "${NET_INCLUDE_DIR}/*.hpp"
)

add_library(Net ${NET_SOURCES})
add_library(Net::Net ALIAS Net)

target_include_directories(Net
    PUBLIC
    "${NET_INCLUDE_DIR}"
    PRIVATE
    "${NET_SOURCE_DIR}"
)

target_compile_features(Net
    PUBLIC
    cxx_std_23
)

set_target_properties(Net PROPERTIES
    OUTPUT_NAME "libnet"
)
