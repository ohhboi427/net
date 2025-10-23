FetchContent_Declare(
    googletest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG "main"
    SYSTEM
)

FetchContent_MakeAvailable(googletest)

include(CTest)
include(GoogleTest)

add_executable(NetTests "net_tests.cpp")

target_link_libraries(NetTests
    PRIVATE
    Net::Net
    GTest::gtest_main
)

gtest_discover_tests(NetTests)
