#include "texture.hpp"

GLuint createTexture(const std::filesystem::path& filePath) {
    int width, height, numComponents;

    #ifdef _MSC_VER
        const std::string& filePathString = (const std::string&) filePath.u8string();
    #else
        const std::string& filePathString = (const std::string&) filePath;
    #endif

    unsigned char* image = stbi_load(filePath.c_str(), &width, &height, &numComponents, STBI_rgb);

    if (image == NULL){
        std::cout << "Cannot load texture: " << filePath << std::endl;
        throw std::runtime_error("Cannot load texture");
    }

    GLuint texture;

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    glBindTexture(GL_TEXTURE_2D, 1);

    stbi_image_free(image);

    return texture;
}