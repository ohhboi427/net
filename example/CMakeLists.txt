add_executable(Example "example.cpp")

target_link_libraries(Example
    PRIVATE
    Net::Net
)
