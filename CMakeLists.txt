cmake_minimum_required(VERSION 3.10)

project(VisualEngine)

INCLUDE_DIRECTORIES(include/)

add_definitions(-D_SHADER_PATH="./shaders/")
add_definitions(-D_MODEL_PATH="./models3d/")

file(GLOB_RECURSE SHADER_FILES *.frag *.vert *.glsl)
aux_source_directory(src/ SRC_LIST)

set(CMAKE_CXX_STANDARD 17)

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

if (WIN32)
# Please correct each of the following variables to much your library locations
	set(LIBRARY_DIRS
	../glfw-3.2.1.bin.WIN32/lib-vc2015
	../glew-2.1.0/lib/Release/Win32
	"../soil/Simple OpenGL Image Library/projects/VC9/Debug"
	../assimp-4.1.0/build/code/Debug
	../assimp-4.1.0/build/contrib/irrXML/Debug
	../assimp-4.1.0/build/contrib/zlib/Debug)

	set(ALL_INCLUDES
	../VisualEngine/include
	../glew-2.1.0/include
	../glfw-3.2.1.bin.WIN32/include
	../glm-0.9.9.4/glm
	../assimp-4.1.0/include
	../assimp-4.1.0/build/include
	"../soil/Simple OpenGL Image Library")

	include_directories(include ${ALL_INCLUDES})
	link_directories(${LIBRARY_DIRS})
elseif (LINUX)
	set(CMAKE_CXX_FLAGS "-pthread")
endif()

add_executable(${PROJECT_NAME} ${SRC_LIST} ${SHADER_FILES})

SET(SHADERS_DIRECTORY shaders)
SET(MODELS_DIRECTORY models3d)
SET(COPY_SOURCE1 "${CMAKE_SOURCE_DIR}/src/${SHADERS_DIRECTORY}")
SET(COPY_SOURCE2 "${CMAKE_SOURCE_DIR}/${MODELS_DIRECTORY}")

if (WIN32)
SET(COPY_DEST "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>")
elseif(LINUX)
SET(COPY_DEST "${CMAKE_CURRENT_BINARY_DIR}")
endif()

message ("current binary dir ${CMAKE_CURRENT_BINARY_DIR}")

if (WIN32)
# on windows, to copy, we need to replace the pesky forward slashes with back slashes
STRING(REGEX REPLACE "/" "\\\\" COPY_SOURCE1 \"${COPY_SOURCE1}\")
STRING(REGEX REPLACE "/" "\\\\" COPY_SOURCE2 \"${COPY_SOURCE2}\")
STRING(REGEX REPLACE "/" "\\\\" COPY_DEST \"${COPY_DEST}\")
endif (WIN32)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${COPY_SOURCE1}
                ${COPY_DEST}/${SHADERS_DIRECTORY})

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${COPY_SOURCE2}
                ${COPY_DEST}/${MODELS_DIRECTORY})

if (WIN32)
	set(ALL_LIBRARIES
	OpenGL32
	glfw3
	glew32
	SOIL
	assimp-vc140-mt
	IrrXML
	zlibstaticd)
	target_link_libraries(${PROJECT_NAME} ${ALL_LIBRARIES})
elseif (LINUX)
	set(CMAKE_CXX_FLAGS "-pthread")
        target_link_libraries(${PROJECT_NAME} GL glfw GLEW dl SOIL assimp entityx libnewton.so)
endif()
