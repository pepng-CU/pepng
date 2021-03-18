#pragma once

#include <memory>
#include <GL/glew.h>

#include "texture.hpp"
#include "../util/cloneable.hpp"

/**
 * The component that hold rendering properties.
 */
class Material : public Cloneable<Material> {
    public:
        /**
         * The texture used.
         */
        std::shared_ptr<Texture> texture;

        /**
         * Shared_ptr of Material.
         */
        static std::shared_ptr<Material> makeMaterial(GLuint shaderProgram, std::shared_ptr<Texture> texture);

        /**
         * Accessor for shader program.
         */
        GLuint getShaderProgram();

    protected:
        virtual Material* cloneImplementation() override;

    private:
        Material(GLuint shaderProgram, std::shared_ptr<Texture> texture);
        Material(const Material& material);

        /**
         * The OpenGL shaderProgram used.
         */
        GLuint shaderProgram;
};

namespace pepng {
    std::shared_ptr<Material> makeMaterial(GLuint shaderProgram, std::shared_ptr<Texture> texture);
};