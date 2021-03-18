#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include <memory>
#include <stb_image.h>
#include <GL/glew.h>

#include "../util/delayed_init.hpp"

/**
 * Handler to read/generate textures.
 */
class Texture : public DelayedInit {
    public:
        /**
         * Default shared_ptr constructor Texture.
         */
        static std::shared_ptr<Texture> makeTexture();

        /**
         * Texture from index.
         */
        static std::shared_ptr<Texture> makeTexture(GLuint textureIndex);

        /**
         * Shared_ptr constructor Texture.
         */
        static std::shared_ptr<Texture> makeTexture(const std::filesystem::path& filePath);

        virtual void delayedInit() override;

        /**
         * Accessor for OpenGL texture index.
         */
        GLuint getIndex();

    protected:
        virtual Texture* cloneImplementation() override;

    private:
        Texture();
        Texture(const Texture& texutre);

        /**
         * The OpenGL texture index.
         */
        GLuint textureIndex;

        /**
         * Pointer to STB image array.
         */
        stbi_uc* image;

        /**
         * Width of image.
         */
        int width;
        
        /**
         * Height of image.
         */
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

    /**
     * Creates texture from textureIndex.
     */
    std::shared_ptr<Texture> makeTexture(GLuint textureIndex);
}
