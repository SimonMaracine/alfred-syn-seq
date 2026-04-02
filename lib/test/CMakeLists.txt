cmake_minimum_required(VERSION 3.20)

add_executable(test "main.cpp")
target_link_libraries(test PRIVATE alfred_lib)
set_target_cpp_mode(test)
