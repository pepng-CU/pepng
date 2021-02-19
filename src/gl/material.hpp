#pragma once

#include <memory>
#include <GL/glew.h>

#include "texture.hpp"

/**
 * The component that hold rendering properties.
 */
class Material {
    public:
        /**
         * Shared_ptr of Material.
         */
        static std::shared_ptr<Material> makeMaterial(GLuint shaderProgram, std::shared_ptr<Texture> texture);

        /**
         * Accessor for shader program.
         */
        GLuint getShaderProgram();

        /**
         * Accessor for texture.
         */
        GLuint getTexture();

        virtual std::shared_ptr<Material> clone();

    private:
        Material(GLuint shaderProgram, std::shared_ptr<Texture> texture);
        Material(const Material& material);

        /**
         * The OpenGL shaderProgram used.
         */
        GLuint shaderProgram;
        /**
         * The texture used.
         */
        std::shared_ptr<Texture> texture;
};

namespace pepng {
    std::shared_ptr<Material> makeMaterial(GLuint shaderProgram, std::shared_ptr<Texture> texture);
};