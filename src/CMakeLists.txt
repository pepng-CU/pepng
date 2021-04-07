file(GLOB_RECURSE SRCS *.cpp)

add_library(${PROJECT_NAME} STATIC ${SRCS})

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/tinyxml2
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/stb
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

if(NOT EMSCRIPTEN)
    target_include_directories(${PROJECT_NAME}
        PUBLIC ${thirdparty-glew_SOURCE_DIR}/include
    )

    target_link_libraries(${PROJECT_NAME} OpenGL::GL glew_s)
endif()

target_link_libraries(${PROJECT_NAME} glm tinyxml2 glfw)

if(IMGUI)
    target_link_libraries(${PROJECT_NAME} imgui)
endif()

if(IMGUI)
    target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI)
endif()

if(EMSCRIPTEN)
    target_compile_definitions(${PROJECT_NAME} PUBLIC EMSCRIPTEN)
endif()

if(EXTRA_OBJECTS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC EXTRA_OBJECTS)
endif()

if(EXTRA_COMPONENTS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC EXTRA_COMPONENTS)
endif()

if(DEBUG_MODEL)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG_MODEL)
endif()