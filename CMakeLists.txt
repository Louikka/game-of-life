cmake_minimum_required(VERSION 4.1)

project(GameOfLife)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)


add_executable(game WIN32)

target_sources(game
PRIVATE
    src/main.c
    src/grid.c
)

target_link_libraries(game
PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

target_compile_options(game
PRIVATE
    -O2 -s
)
