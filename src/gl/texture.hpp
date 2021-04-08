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
        static std::shared_ptr<Texture> make_texture();

        /**
         * Texture from index.
         */
        static std::shared_ptr<Texture> make_texture(GLuint textureIndex);

        /**
         * Shared_ptr constructor Texture.
         */
        static std::shared_ptr<Texture> make_texture(const std::filesystem::path& filePath);

        virtual void delayed_init() override;

        /**
         * Accessor for OpenGL texture index.
         */
        inline GLuint gl_index() {
            if(!this->_is_init) this->delayed_init();

            return this->__texture_index;
        }

    protected:
        virtual Texture* clone_implementation() override;

        Texture();
        Texture(const Texture& texutre);

    private:

        /**
         * The OpenGL texture index.
         */
        GLuint __texture_index;

        /**
         * Pointer to STB image array.
         */
        stbi_uc* __image;

        /**
         * Width of image.
         */
        int __width;
        
        /**
         * Height of image.
         */
        int __height;
};

namespace pepng {
    /**
     * Creates and binds texture.
     */
    std::shared_ptr<Texture> make_texture(const std::filesystem::path& filePath);

    /**
     * Creates empty void texture.
     */
    std::shared_ptr<Texture> make_texture();

    /**
     * Creates texture from textureIndex.
     */
    std::shared_ptr<Texture> make_texture(GLuint textureIndex);
}
