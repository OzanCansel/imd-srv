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
    dictionary/dictionary.hpp
    dictionary/dictionary.cpp
)

target_include_directories(
    srv
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
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