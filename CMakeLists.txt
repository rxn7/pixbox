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

target_compile_definitions(pixbox PRIVATE GRID_WIDTH=256 GRID_HEIGHT=256)
target_compile_options(pixbox PUBLIC -O4 -Wall -pedantic -g)

add_custom_target(copy_assets
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/assets
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)

add_dependencies(pixbox copy_assets)
