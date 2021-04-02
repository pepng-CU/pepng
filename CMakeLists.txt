cmake_minimum_required(VERSION 3.11)

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Debug)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

project(pepng)

include(ExternalProject)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH ${CMAKE_SOURCE_DIR}/dist FORCE)

if(WIN32)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Debug;Release" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Submodules
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

include(BuildGLEW)
include(BuildGLM)

add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/imgui-cmake)
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/tinyxml2)
add_subdirectory(${CMAKE_SOURCE_DIR}/src)

if(NOT NO_EXEC)
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)

    include(BuildGLFW)

    set(EXEC main)

    add_executable(${EXEC} drivers/main.cpp)

    target_include_directories(${EXEC} 
        PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/stb
        PUBLIC ${CMAKE_SOURCE_DIR}/include
    )

    target_link_libraries(${EXEC} OpenGL::GL glew_s glfw glm imgui ${PROJECT_NAME})
endif()