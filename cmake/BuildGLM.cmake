add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/thirdparty/include/glm
    COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_SOURCE_DIR}/thirdparty/glm-0.9.9.8.zip glm/glm
    COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/thirdparty/build/glm/glm ${CMAKE_BINARY_DIR}/thirdparty/include/glm
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/thirdparty/build
    COMMENT "Extracting GLM and moving library"
)

add_custom_target(unzip_glm
    DEPENDS ${CMAKE_BINARY_DIR}/thirdparty/include/glm
)

add_library(glm INTERFACE)

set_target_properties(glm PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/thirdparty/include
)

add_dependencies(glm unzip_glm)