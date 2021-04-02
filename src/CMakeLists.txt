file(GLOB_RECURSE SRCS *.cpp)

add_library(${PROJECT_NAME} STATIC ${SRCS})

target_include_directories(${PROJECT_NAME} 
    INTERFACE $<BUILD_INTERFACE:${thirdparty-glew_SOURCE_DIR}/include>
    PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/stb
)

target_link_libraries(${PROJECT_NAME} glm glfw tinyxml2)

if(IMGUI)
    target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI)
endif()

if(EXTRA_OBJECTS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC EXTRA_OBJECTS)
endif()

if(EXTRA_COMPONENTS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC EXTRA_COMPONENTS)
endif()

if(DEBUG_COLLADA)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG_MODEL)
endif()

if(IMGUI)
    target_link_libraries(${PROJECT_NAME} imgui)
endif()