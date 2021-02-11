#include "texture.hpp"

GLuint pepng::makeTexture(const std::filesystem::path& filePath) {
    int width, height, numComponents;

    #ifdef _MSC_VER
        const std::string& filePathString = (const std::string&) filePath.u8string();
    #else
        const std::string& filePathString = (const std::string&) filePath;
    #endif

    stbi_set_flip_vertically_on_load(true);
    
    unsigned char* image = stbi_load(filePath.c_str(), &width, &height, &numComponents, STBI_rgb);

    if (image == NULL){
        throw std::runtime_error("Cannot load texture: " + filePath.string());
    }

    GLuint texture;

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);

    return texture;
}