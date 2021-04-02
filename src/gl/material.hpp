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
        static std::shared_ptr<Material> make_material(GLuint shaderProgram, std::shared_ptr<Texture> texture);

        /**
         * Accessor for shader program.
         */
        inline GLuint shader_program() { return this->__shader_program; }

    protected:
        virtual Material* clone_implementation() override;

    private:
        Material(GLuint shaderProgram, std::shared_ptr<Texture> texture);
        Material(const Material& material);

        /**
         * The OpenGL shaderProgram used.
         */
        GLuint __shader_program;
};

namespace pepng {
    std::shared_ptr<Material> make_material(GLuint shaderProgram, std::shared_ptr<Texture> texture);
};