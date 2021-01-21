# Build GLEW
ExternalProject_Add(project_glew
    PREFIX ${CMAKE_BINARY_DIR}/thirdparty/build/glew
    URL ${CMAKE_SOURCE_DIR}/thirdparty/glew-2.1.0.zip
    SOURCE_SUBDIR build/cmake
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/thirdparty
    INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty
)

ExternalProject_Get_Property(project_glew install_dir)

# Create include directory for GLFW in build directory
file(MAKE_DIRECTORY ${install_dir}/include)

# Add imported GLEW library
add_library(GLEW STATIC IMPORTED)

# Set GLEW library properties for linking
if(WIN32)
    set_target_properties(GLEW PROPERTIES
        IMPORTED_LOCATION ${install_dir}/lib/libglew32d.lib
        INTERFACE_INCLUDE_DIRECTORIES ${install_dir}/include
    )
else()
    set_target_properties(GLEW PROPERTIES
        IMPORTED_LOCATION ${install_dir}/lib/libGLEW.a
        INTERFACE_INCLUDE_DIRECTORIES ${install_dir}/include
        INTERFACE_LINK_LIBRARIES "/usr/lib/libGL.so;/usr/lib/libGLU.so"
    )
endif()

# Set external GLFW project as dependency on imported target
add_dependencies(GLEW project_glew)