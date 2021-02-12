#pragma once

#include <memory>
#include <GL/glew.h>

#include "texture.hpp"

class Material {
    public:
        static std::shared_ptr<Material> makeMaterial(GLuint shaderProgram, std::shared_ptr<Texture> texture);

        GLuint getShaderProgram();

        GLuint getTexture();

    private:
        Material(GLuint shaderProgram, std::shared_ptr<Texture> texture);

        GLuint shaderProgram;
        std::shared_ptr<Texture> texture;
};

namespace pepng {
    std::shared_ptr<Material> makeMaterial(GLuint shaderProgram, std::shared_ptr<Texture> texture);
};