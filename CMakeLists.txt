cmake_minimum_required(VERSION 3.28)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(pixbox VERSION 0.0 LANGUAGES C)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.c)
add_executable(pixbox ${sources})

add_subdirectory(vendor/SDL3 EXCLUDE_FROM_ALL)
target_link_libraries(pixbox PRIVATE SDL3::SDL3)
add_subdirectory(vendor/SDL3_ttf EXCLUDE_FROM_ALL)
target_link_libraries(pixbox PRIVATE SDL3_ttf::SDL3_ttf)

target_link_libraries(pixbox PRIVATE m)

target_compile_definitions(pixbox PRIVATE GRID_WIDTH=640 GRID_HEIGHT=480)
target_compile_options(pixbox PUBLIC -O3 -Wall -pedantic -g)
