add_executable(Client "client.cpp")
add_executable(Server "server.cpp")

target_link_libraries(Client
    PRIVATE
    Net::Net
)

target_link_libraries(Server
    PRIVATE
    Net::Net
)
