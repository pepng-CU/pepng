cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

project(COMP371)

include(ExternalProject)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH ${CMAKE_SOURCE_DIR}/dist FORCE)

if(WIN32)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Debug;Release" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

if (NIXOS)
    find_package(GLEW REQUIRED)

    find_package(glfw3 REQUIRED)
else()
    include(BuildGLEW)
    
    include(BuildGLFW)
endif()

include(BuildGLM)

add_subdirectory(src)

add_subdirectory(drivers)

file(COPY shaders DESTINATION ${PROJECT_SOURCE_DIR}/bin)