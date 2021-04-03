cmake_minimum_required(VERSION 3.11)

project(pepng)

set (CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

###########
# Options #
###########
option(DEBUG_MODE "Enables CMake debug mode." ON)
option(DEBUG_MODEL "Enables logging when loading model(s)." OFF)
option(DRIVER "Builds the driver file." ON)
option(EXTRA_OBJECTS "Includes objects in extra folder." ON)
option(EXTRA_COMPONENTS "Includes components in extra folder." ON)
option(IMGUI "Enables IMGUI." ON)

#################
# Configuration #
#################

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
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

include(BuildGLEW)
include(BuildGLFW)
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

    target_include_directories(${EXEC} 
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_link_libraries(${EXEC} OpenGL::GL glew_s ${PROJECT_NAME})
endif()