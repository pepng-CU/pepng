#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include <stb_image.h>
#include <GL/glew.h>

/**
 * Creates and binds texture.
 */
GLuint createTexture(const std::filesystem::path& filePath);