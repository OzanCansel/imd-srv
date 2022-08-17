cmake_minimum_required( VERSION 3.10 )
project( srv )

find_package(
    Boost

    COMPONENTS
    thread
    program_options

    REQUIRED
)

add_executable(
    srv
    main.cpp
    net/connection.hpp
    net/connection.cpp
    net/server.hpp
    net/server.cpp
)

target_link_libraries(
    srv
    Boost::program_options
    Boost::thread
)

target_compile_features(
    srv
    PRIVATE
    cxx_std_17
)