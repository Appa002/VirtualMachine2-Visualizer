cmake_minimum_required(VERSION 3.11)
project(vm_visualizer C)

set(CMAKE_C_STANDARD 11)

file(GLOB headers ${CMAKE_SOURCE_DIR}/header/*.h)

add_executable(vm_visualizer main.c ${headers})

target_link_libraries(vm_visualizer ncurses m)