cmake_minimum_required(VERSION 3.21)
project(simulation)

set(CMAKE_CXX_STANDARD 20)
find_package (Eigen3 3.4 REQUIRED NO_MODULE)

add_executable(simulation main.cpp basic_impl/Peers.h)

# needs to be first subdirectory to add, since the concrete implementations libaries are linked to this
add_subdirectory(lib)

add_subdirectory(basic_impl)

target_link_libraries(simulation PUBLIC basic_impl)
