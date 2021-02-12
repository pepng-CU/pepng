#include "material.hpp"

Material::Material(GLuint shaderProgram, std::shared_ptr<Texture> texture) : 
    shaderProgram(shaderProgram),
    texture(texture)
{}

std::shared_ptr<Material> Material::makeMaterial(GLuint shaderProgram, std::shared_ptr<Texture> texture) {
    std::shared_ptr<Material> material(new Material(shaderProgram, texture));

    return material;
}

GLuint Material::getShaderProgram() {
    return this->shaderProgram;
}

GLuint Material::getTexture() {
    return this->texture->getIndex();
}

namespace pepng {
    std::shared_ptr<Material> makeMaterial(GLuint shaderProgram, std::shared_ptr<Texture> texture) {
        return Material::makeMaterial(shaderProgram, texture);
    }
};