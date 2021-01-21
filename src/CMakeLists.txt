file(GLOB_RECURSE HEADERS
    "*.hpp"
)

file(GLOB_RECURSE SOURCES
    "*.cpp"
)

add_library(${PROJECT_NAME}
    STATIC
        ${HEADERS} 
        ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)