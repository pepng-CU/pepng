file(GLOB_RECURSE SRCS *.cpp)

add_library(${PROJECT_NAME} STATIC ${SRCS})

target_include_directories(${PROJECT_NAME} 
    INTERFACE $<BUILD_INTERFACE:${thirdparty-glew_SOURCE_DIR}/include>
    PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/stb
)

target_link_libraries(${PROJECT_NAME} glm imgui tinyxml2)