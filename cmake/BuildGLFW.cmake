# Build GLFW
include(FetchContent)

mark_as_advanced(FETCHCONTENT_QUIET)
mark_as_advanced(FETCHCONTENT_FULLY_DISCONNECTED)
mark_as_advanced(FETCHCONTENT_UPDATES_DISCONNECTED)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON) # speed up config

set(THIRDPARTY thirdparty-glfw)

FetchContent_Declare(${THIRDPARTY}
    URL ${CMAKE_SOURCE_DIR}/thirdparty/glfw-3.3.2.zip
)

mark_as_advanced(FETCHCONTENT_BASE_DIR)

FetchContent_GetProperties(${THIRDPARTY})

string(TOUPPER ${THIRDPARTY} UTHIRDPARTY)

mark_as_advanced(FETCHCONTENT_SOURCE_DIR_${UTHIRDPARTY})
mark_as_advanced(FETCHCONTENT_UPDATES_DISCONNECTED_${UTHIRDPARTY})

if(NOT ${THIRDPARTY}_POPULATED)
    FetchContent_Populate(${THIRDPARTY})

    add_subdirectory(
        ${${THIRDPARTY}_SOURCE_DIR}
        ${${THIRDPARTY}_BINARY_DIR}
        EXCLUDE_FROM_ALL
    )

endif()

# hide GLFW options
mark_as_advanced(BUILD_SHARED_LIBS)
mark_as_advanced(GLFW_BUILD_EXAMPLES)
mark_as_advanced(GLFW_BUILD_TESTS)
mark_as_advanced(GLFW_BUILD_DOCS)
mark_as_advanced(GLFW_INSTALL)
mark_as_advanced(GLFW_VULKAN_STATIC)
mark_as_advanced(GLFW_USE_OSMESA)
mark_as_advanced(GLFW_USE_HYBRID_HPG)
mark_as_advanced(GLFW_USE_WAYLAND)
mark_as_advanced(USE_MSVC_RUNTIME_LIBRARY_DLL)


set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
