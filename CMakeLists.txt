cmake_minimum_required(VERSION 3.11)

project(pepng)

set (CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

###########
# Options #
###########
option(DEBUG_MODE "Enables CMake debug mode." ON)
option(DEBUG_MODEL "Enables logging when loading model(s)." ON)
option(DRIVER "Builds the driver file." ON)
option(EXTRA_OBJECTS "Includes objects in extra folder." ON)
option(EXTRA_COMPONENTS "Includes components in extra folder." ON)
option(IMGUI "Enables IMGUI." ON)

#################
# Configuration #
#################

# Checks if Emscripten.
if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
    set(EMSCRIPTEN ON)
else()
    set(EMSCRIPTEN OFF)
endif()

# Build tinyxml as static instead of shared.
set(BUILD_SHARED_LIBS OFF)

# Prevents tinyxml tests.
set(BUILD_TESTS OFF)

#########
# CMake #
#########
include(ExternalProject)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/dist CACHE PATH ${CMAKE_CURRENT_SOURCE_DIR}/dist FORCE)

if(WIN32)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Debug;Release" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

##################
# Git Submodules #
##################
find_package(Git QUIET)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Adds submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodules update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMODULE_RESULT)
        if(NOT GIT_SUBMODULE_RESULT EQUAL "0")
            message(FALT_ERROR "Unable to fetch submodules wtih ${GIT_EXECUTABLE}. Error code ${GIT_SUBMODULE_RESULT}")
        endif()
    endif()
endif() 

#############
# Libraries #
#############

if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s USE_WEBGL2=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s EXCEPTION_CATCHING_ALLOWED=[..] -s ALLOW_MEMORY_GROWTH=1")
else()
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)

    include(BuildGLFW)
    include(BuildGLEW)
endif()

include(BuildGLM)

if(IMGUI)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui-cmake)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/tinyxml2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

##########
# Driver #
##########
if(DRIVER)
    set(EXEC main)

    add_executable(${EXEC} drivers/main.cpp)

    if(EMSCRIPTEN)
        set(CMAKE_EXECUTABLE_SUFFIX ".html")

        file(GLOB_RECURSE shader_files "./shaders/*")
        foreach(file ${shader_files})
            file(RELATIVE_PATH relative_file ${CMAKE_SOURCE_DIR} ${file})
            string(APPEND CMAKE_CXX_FLAGS " --preload-file ${file}@/${relative_file}")
            message(STATUS CMAKE_CXX_FLAGS " --preload-file ${file}@/${relative_file}")
        endforeach()

        file(GLOB_RECURSE texture_files "./textures/*")
        foreach(file ${texture_files})
            file(RELATIVE_PATH relative_file ${CMAKE_SOURCE_DIR} ${file})
            string(APPEND CMAKE_CXX_FLAGS " --preload-file ${file}@/${relative_file}")
            message(STATUS CMAKE_CXX_FLAGS " --preload-file ${file}@/${relative_file}")
        endforeach()

        file(GLOB_RECURSE model_files "./models/*")
        foreach(file ${model_files})
            file(RELATIVE_PATH relative_file ${CMAKE_SOURCE_DIR} ${file})
            string(APPEND CMAKE_CXX_FLAGS " --preload-file ${file}@/${relative_file}")
            message(STATUS CMAKE_CXX_FLAGS " --preload-file ${file}@/${relative_file}")
        endforeach()
    else()
        target_link_libraries(${EXEC} OpenGL::GL glew_s)
    endif()

    target_link_libraries(${EXEC} glm ${PROJECT_NAME})
endif()