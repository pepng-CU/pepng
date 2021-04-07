cmake_minimum_required(VERSION 3.11)

project(pepng)

set (CMAKE_CXX_STANDARD 20)

###########
# Options #
###########

option(DEBUG_MODEL "Enables logging when loading model(s)." OFF)
option(EXTRA_OBJECTS "Includes objects in extra folder." OFF)
option(EXTRA_COMPONENTS "Includes components in extra folder." OFF)
option(IMGUI "Enables IMGUI." ON)

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

if(NOT EMSCRIPTEN)
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

#################
# Configuration #
#################

# Build tinyxml as static instead of shared.
set(BUILD_SHARED_LIBS OFF)

# Prevents tinyxml tests.
set(BUILD_TESTS OFF)