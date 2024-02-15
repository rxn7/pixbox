cmake_minimum_required(VERSION 3.28)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(pixbox VERSION 0.0 LANGUAGES C)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.c)
add_executable(pixbox ${sources})

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
target_link_libraries(pixbox PRIVATE SDL3::SDL3)

target_compile_definitions(pixbox PRIVATE GRID_WIDTH=640 GRID_HEIGHT=480)
target_compile_options(pixbox PUBLIC -O3 -Wall -pedantic)
