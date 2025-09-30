cmake_minimum_required(VERSION 4.0)
project(BatchedRenderTest)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC
    src/imgui/*.cpp
    src/imgui/*.h
    src/*.h
    src/*.cpp
)

add_executable(BatchedRenderTest ${SRC})

target_include_directories(BatchedRenderTest PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/include
)

target_link_directories(BatchedRenderTest PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/lib/x64
)

target_link_libraries(BatchedRenderTest PRIVATE
    SDL3
    SDL3_image
    glew32s
    opengl32
)
