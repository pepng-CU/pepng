# Build GLM
include(FetchContent)

mark_as_advanced(FETCHCONTENT_QUIET)
mark_as_advanced(FETCHCONTENT_FULLY_DISCONNECTED)
mark_as_advanced(FETCHCONTENT_UPDATES_DISCONNECTED)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON) # speed up config

set(THIRDPARTY thirdparty-glm)

FetchContent_Declare(${THIRDPARTY}
    URL ${CMAKE_SOURCE_DIR}/thirdparty/glm-0.9.9.8.zip
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

set(BUILD_STATIC_LIBS OFF)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)

