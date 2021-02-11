#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include <stb_image.h>
#include <GL/glew.h>

namespace pepng {
    /**
     * Creates and binds texture.
     */
    GLuint makeTexture(const std::filesystem::path& filePath);
}
