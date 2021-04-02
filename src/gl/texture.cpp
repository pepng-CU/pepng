#include "texture.hpp"

Texture::Texture() :
    DelayedInit(),
    __texture_index(0),
    __width(0),
    __height(0),
    __image(nullptr)
{}

// TODO: Make a deep clone of the image?
Texture::Texture(const Texture& texture) : 
    DelayedInit(texture),
    __texture_index(texture.__texture_index),
    __width(texture.__width),
    __height(texture.__height),
    __image(texture.__image)
{}

void Texture::delayed_init() {
    if(this->_is_init) {
        return;
    }

    this->_is_init = true;

    glGenTextures(1, &this->__texture_index);

    glBindTexture(GL_TEXTURE_2D, this->__texture_index);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->__width, this->__height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->__image);

    stbi_image_free(this->__image);
}

std::shared_ptr<Texture> Texture::make_texture(const std::filesystem::path& filePath) {
    std::shared_ptr<Texture> texture(new Texture());

    #ifdef _MSC_VER
        const std::string& filePathString = (const std::string&) filePath.u8string();
    #else
        const std::string& filePathString = (const std::string&) filePath;
    #endif

    stbi_set_flip_vertically_on_load_thread(true);

    int numComponents;
    texture->__image = stbi_load(filePathString.c_str(), &texture->__width, &texture->__height, &numComponents, STBI_rgb_alpha);

    if (texture->__image == nullptr){
        throw std::runtime_error("Cannot load texture: " + filePath.string());
    }

    return texture;
}

std::shared_ptr<Texture> Texture::make_texture() {
    std::shared_ptr<Texture> texture(new Texture());

    texture->_is_init = true;
    texture->__texture_index = 1;

    return texture;
}

std::shared_ptr<Texture> Texture::make_texture(GLuint textureIndex) {
    std::shared_ptr<Texture> texture(new Texture());

    texture->_is_init = true;
    texture->__texture_index = textureIndex;

    return texture;
}

Texture* Texture::clone_implementation() {
    return new Texture(*this);
}

namespace pepng {
    std::shared_ptr<Texture> make_texture(const std::filesystem::path& filePath) {
        return Texture::make_texture(filePath);
    }

    std::shared_ptr<Texture> make_texture() {
        return Texture::make_texture();
    }

    std::shared_ptr<Texture> make_texture(GLuint textureIndex) {
        return Texture::make_texture(textureIndex);
    }
}