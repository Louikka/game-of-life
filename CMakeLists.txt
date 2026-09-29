cmake_minimum_required(VERSION 4.1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)

if(WIN32)
    #set(VCPKG_TARGET_TRIPLET x64-windows-static)
endif()


project(GameOfLife)


find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)


add_executable(game WIN32)

target_sources(game
PRIVATE
    src/main.c
    src/game_grid.c
    src/text.c
    src/draw.c
    src/presets.c
    src/lib/timer.c
)

target_link_libraries(game
PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

target_compile_options(game PRIVATE -Wall -O2 -s)
target_link_options(game PRIVATE -static)


add_custom_command(
    TARGET game
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/src/assets/fonts/JetBrains_Mono/static/JetBrainsMono-Regular.ttf"
        "$<TARGET_FILE_DIR:game>/JetBrainsMono-Regular.ttf"
    COMMENT "Copying assets..."
)
