cmake_minimum_required(VERSION 3.0.0)
project(soft_renderer)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(soft_renderer src/main.c)
target_link_libraries(soft_renderer SDL2::SDL2-static)
