file(GLOB_RECURSE FILES 
    "*.cpp"
)

message(STATUS "${FILES}")

foreach(FILE ${FILES})
    get_filename_component(FILE_NAME_EXT "${FILE}" NAME)

    string(REGEX MATCH "^(.*)\\.[^.]*$" dummy ${FILE_NAME_EXT})

    set(FILE_NAME ${CMAKE_MATCH_1})

    add_executable(${PROJECT_NAME}_${FILE_NAME}
        "${FILE_NAME}.cpp"
    )

    target_include_directories(${PROJECT_NAME}_${FILE_NAME} 
        PUBLIC 
        include
    )

    target_link_libraries(${PROJECT_NAME}_${FILE_NAME}
        PUBLIC
            ${PROJECT_NAME}
            OpenGL::GL 
            GLEW 
            glfw 
            glm
    )
endforeach()