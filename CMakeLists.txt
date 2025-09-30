cmake_minimum_required(VERSION 4.0)
project(game)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC
    src/imgui/*.cpp
    src/imgui/*.h
    src/*.h
    src/*.cpp
)

add_executable(game ${SRC})

target_include_directories(game PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/include
)

target_link_directories(game PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/lib/x64
)

target_link_libraries(game PRIVATE
    SDL3
    SDL3_image
    glew32s
    opengl32
)
