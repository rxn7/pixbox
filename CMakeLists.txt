cmake_minimum_required(VERSION 3.28)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(pixbox VERSION 0.0 LANGUAGES C)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.c)
add_executable(pixbox ${sources})

# Link sdl3
add_subdirectory(vendor/SDL3 EXCLUDE_FROM_ALL)
target_link_libraries(pixbox PRIVATE SDL3::SDL3)

# Link sdl3_ttf
add_subdirectory(vendor/SDL3_ttf EXCLUDE_FROM_ALL)
target_link_libraries(pixbox PRIVATE SDL3_ttf::SDL3_ttf)

# Link libm
target_link_libraries(pixbox PRIVATE m)

target_compile_definitions(pixbox PRIVATE GRID_WIDTH=256 GRID_HEIGHT=256)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Copy assets to the output directory

if(WIN32)
    add_custom_command(
        TARGET pixbox POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:pixbox>
        VERBATIM
    )
    add_custom_command(
        TARGET pixbox POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared> $<TARGET_FILE_DIR:pixbox>
        VERBATIM
    )
endif()

add_custom_target(copy_assets
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/assets
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(pixbox copy_assets)
