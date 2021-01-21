# Build GLFW
ExternalProject_Add(project_glfw
    PREFIX ${CMAKE_BINARY_DIR}/thirdparty/build/glfw
    URL ${CMAKE_SOURCE_DIR}/thirdparty/glfw-3.3.2.zip
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/thirdparty
               -DGLFW_BUILD_EXAMPLES=OFF
               -DGLFW_BUILD_TESTS=OFF
               -DGLFW_BUILD_DOCS=OFF
    INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty
)

ExternalProject_Get_Property(project_glfw install_dir)

# Create include directory for GLFW in build directory
file(MAKE_DIRECTORY ${install_dir}/include)

# Add imported GLFW library
add_library(glfw STATIC IMPORTED)

# Set GLFW library properties for linking
if(WIN32)
    set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION ${install_dir}/lib/glfw3.lib
        INTERFACE_INCLUDE_DIRECTORIES ${install_dir}/include
    )
else()
    set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION ${install_dir}/lib/libglfw3.a
        INTERFACE_INCLUDE_DIRECTORIES ${install_dir}/include
        INTERFACE_LINK_LIBRARIES "/usr/lib/librt.so;/usr/lib/libm.so;\$<LINK_ONLY:dl>;/usr/lib/libX11.so;\$<LINK_ONLY:-lpthread>"
    )
endif()

# Set external GLFW project as dependency on imported target
add_dependencies(glfw project_glfw)