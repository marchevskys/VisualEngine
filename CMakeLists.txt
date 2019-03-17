cmake_minimum_required(VERSION 3.10)

project(VisualEngine)

INCLUDE_DIRECTORIES(include/)

add_definitions(-D_SHADER_PATH="../VisualEngine/src/shaders/")
add_definitions(-D_MODEL_PATH="../VisualEngine/models3d/")

file(GLOB_RECURSE SHADER_FILES *.frag *.vert *.glsl)
aux_source_directory(src/ SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST} ${SHADER_FILES})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-pthread")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES /usr/local/lib ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} GL glfw GLEW dl SOIL assimp)
