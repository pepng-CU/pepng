#include "texture.hpp"

Texture::Texture() :
    DelayedInit(),
    textureIndex(0),
    width(0),
    height(0),
    image(nullptr)
{}

// TODO: Make a deep clone of the image?
Texture::Texture(const Texture& texture) : 
    DelayedInit(texture),
    textureIndex(texture.textureIndex),
    width(texture.width),
    height(texture.height),
    image(texture.image)
{}

GLuint Texture::getIndex() {
    if(!this->isInit) {
        this->delayedInit();
    }

    return this->textureIndex;
}

void Texture::delayedInit() {
    if(this->isInit) {
        return;
    }

    this->isInit = true;

    glGenTextures(1, &this->textureIndex);

    glBindTexture(GL_TEXTURE_2D, this->textureIndex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->image);

    stbi_image_free(this->image);
}

std::shared_ptr<Texture> Texture::makeTexture(const std::filesystem::path& filePath) {
    std::shared_ptr<Texture> texture(new Texture());

    #ifdef _MSC_VER
        const std::string& filePathString = (const std::string&) filePath.u8string();
    #else
        const std::string& filePathString = (const std::string&) filePath;
    #endif

    stbi_set_flip_vertically_on_load_thread(true);

    int numComponents;
    texture->image = stbi_load(filePathString.c_str(), &texture->width, &texture->height, &numComponents, STBI_rgb_alpha);

    if (texture->image == nullptr){
        throw std::runtime_error("Cannot load texture: " + filePath.string());
    }

    return texture;
}

std::shared_ptr<Texture> Texture::makeTexture() {
    std::shared_ptr<Texture> texture(new Texture());

    texture->isInit = true;
    texture->textureIndex = 1;

    return texture;
}

Texture* Texture::cloneImplementation() {
    return new Texture(*this);
}

namespace pepng {
    std::shared_ptr<Texture> makeTexture(const std::filesystem::path& filePath) {
        return Texture::makeTexture(filePath);
    }

    std::shared_ptr<Texture> makeTexture() {
        return Texture::makeTexture();
    }
}