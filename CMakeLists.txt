cmake_minimum_required(VERSION 3.0.0)
project(soft_renderer)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/include)

configure_file(base_scene.obj base_scene.obj COPYONLY)

add_executable(soft_renderer src/main.c src/vec3.c src/ccanvas.c src/scene.c src/point.c src/camera.c)
if(WIN32)
    target_link_libraries(soft_renderer SDL2::SDL2-static ${SDL2_LIBRARIES})
else()
    target_link_libraries(soft_renderer ${SDL2_LIBRARIES})
endif()

target_link_libraries(soft_renderer m)