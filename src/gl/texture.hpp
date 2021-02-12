#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include <memory>
#include <stb_image.h>
#include <GL/glew.h>

#include "../util/delayed_init.hpp"

class Texture : public DelayedInit {
    public:
        static std::shared_ptr<Texture> makeTexture();

        static std::shared_ptr<Texture> makeTexture(const std::filesystem::path& filePath);

        virtual void delayedInit() override;

        GLuint getIndex();

    private:
        Texture() {};

        GLuint textureIndex;

        stbi_uc* image;

        int width;
        
        int height;
};

namespace pepng {
    /**
     * Creates and binds texture.
     */
    std::shared_ptr<Texture> makeTexture(const std::filesystem::path& filePath);

    /**
     * Creates empty void texture.
     */
    std::shared_ptr<Texture> makeTexture();
}
