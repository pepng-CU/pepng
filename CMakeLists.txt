cmake_minimum_required(VERSION 3.11)

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Debug)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

project(COMP371)

include(ExternalProject)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH ${CMAKE_SOURCE_DIR}/dist FORCE)

if(WIN32)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Debug;Release" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

include(BuildGLEW)
include(BuildGLFW)
include(BuildGLM)

add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/imgui-cmake)

set(EXEC main)

file(GLOB_RECURSE SRC src/*.cpp)

add_executable(${EXEC} ${SRC})

include_directories(${CMAKE_SOURCE_DIR}/thirdparty/stb)

link_directories(${EXEC} ${CMAKE_SOURCE_DIR}/thirdparty/stb) 

target_link_libraries(${EXEC} OpenGL::GL glew_s glfw glm imgui)

list(APPEND BIN ${EXEC})

install(TARGETS ${BIN} DESTINATION ${CMAKE_INSTALL_PREFIX})