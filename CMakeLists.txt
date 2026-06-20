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


add_custom_command(
    TARGET game
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/JetBrains_Mono/static/JetBrainsMono-Regular.ttf"
        "$<TARGET_FILE_DIR:game>/JetBrainsMono-Regular.ttf"
    COMMENT "Copying assets..."
)

add_custom_command(
    TARGET game
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3::SDL3>"
        "$<TARGET_FILE_DIR:game>"
    COMMENT "Copying SDL..."
)

add_custom_command(
    TARGET game
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3_ttf::SDL3_ttf>"
        "$<TARGET_FILE_DIR:game>"
    COMMENT "Copying SDL_ttf..."
)
