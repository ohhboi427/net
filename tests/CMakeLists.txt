FetchContent_Declare(
    googletest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG "main"
    SYSTEM
)

FetchContent_MakeAvailable(googletest)

include(CTest)
include(GoogleTest)

file(GLOB_RECURSE NET_TEST_SOURCES CONFIGURE_DEPENDS
    "*.cpp"
)

add_executable(NetTests ${NET_TEST_SOURCES})

target_link_libraries(NetTests
    PRIVATE
    Net::Net
    GTest::gtest_main
)

get_target_property(PRIVATE_INCLUDE_DIRS Net::Net INCLUDE_DIRECTORIES)
target_include_directories(NetTests PRIVATE ${PRIVATE_INCLUDE_DIRS})

gtest_discover_tests(NetTests)
