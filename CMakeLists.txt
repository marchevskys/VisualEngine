cmake_minimum_required(VERSION 3.10)

project(VisualEngine)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (LINUX)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(ENTX REQUIRED entityx)
pkg_check_modules(GL REQUIRED gl)
pkg_check_modules(GLEW REQUIRED glew)
endif()

INCLUDE_DIRECTORIES(include/)

add_definitions(-D_SHADER_PATH="./shaders/")
add_definitions(-D_MODEL_PATH="./models3d/")

file(GLOB_RECURSE SHADER_FILES *.frag *.vert *.glsl)
aux_source_directory(src/ SRC_LIST)

set(CMAKE_CXX_STANDARD 17)


if (WIN32)
# Please correct each of the following variables to much your library locations
    set(LIBRARY_DIRS
    ../glfw-3.2.1.bin.WIN64/lib-vc2015
    ../glew-2.1.0/lib/Release/x64
    "../soil/Simple OpenGL Image Library/projects/VC9/x64/Release"
    ../assimp-4.1.0/build64/code/Release
    ../assimp-4.1.0/build64/contrib/irrXML/Release
    ../assimp-4.1.0/build64/contrib/zlib/Release
    ../entityx/build64/Release
    ../newton-dynamics/sdk/projects/visualStudio_2015_dll/x64/dContainers/release_double
    ../newton-dynamics/sdk/projects/visualStudio_2015_dll/x64/dCustomJoints/release_double
    ../newton-dynamics/sdk/projects/visualStudio_2015_dll/x64/dScene/release_double
    ../newton-dynamics/sdk/projects/visualStudio_2015_dll/x64/dVehicle/release_double
    ../newton-dynamics/sdk/projects/visualStudio_2015_dll/x64/dNewton/release_double
    ../newton-dynamics/sdk/projects/visualStudio_2015_dll/x64/newton/release_double)

    set(ALL_INCLUDES
    ../VisualEngine/include
    ../glew-2.1.0/include
    ../glfw-3.2.1.bin.WIN32/include
    ../glm-0.9.9.4/glm
    ../assimp-4.1.0/include
    ../assimp-4.1.0/build/include
    "../soil/Simple OpenGL Image Library"
    ../entityx
    ../newton-dynamics/sdk)

    include_directories(include ${ALL_INCLUDES})
    link_directories(${LIBRARY_DIRS})
elseif (LINUX)
    set(CMAKE_CXX_FLAGS "-pthread")
endif()

add_executable(${PROJECT_NAME} ${SRC_LIST} ${SHADER_FILES})
add_subdirectory(imgui)

SET(SHADERS_DIRECTORY src/shaders)
SET(MODELS_DIRECTORY models3d)
SET(LEVELS_DIRECTORY levels)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/${SHADERS_DIRECTORY}
            ${CMAKE_CURRENT_BINARY_DIR}/${SHADERS_DIRECTORY})

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/${MODELS_DIRECTORY}
            ${CMAKE_CURRENT_BINARY_DIR}/${MODELS_DIRECTORY})
            
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/${LEVELS_DIRECTORY}
            ${CMAKE_CURRENT_BINARY_DIR}/${LEVELS_DIRECTORY})

if (WIN32)
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/${SHADERS_DIRECTORY}
            ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/${SHADERS_DIRECTORY})

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/${MODELS_DIRECTORY}
            ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/${MODELS_DIRECTORY})

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/${LEVELS_DIRECTORY}
            ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/${LEVELS_DIRECTORY})
endif()

if (WIN32)
    set(ALL_LIBRARIES
        OpenGL32
        glfw3
        glew32
        SOIL
        assimp-vc140-mt
        IrrXML
        zlibstatic
        entityx
        Newton
        dNewton
        dCustomJoints
        dContainers
        dScene
        dVehicle
        imgui_lib
        )
    target_link_libraries(${PROJECT_NAME} ${ALL_LIBRARIES})
elseif (LINUX)
    set(CMAKE_CXX_FLAGS "-pthread")
    target_link_libraries(${PROJECT_NAME}
        ${GL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES}
        dl SOIL assimp
        ${ENTX_LIBRARIES} imgui_lib libnewton.so)
endif()
