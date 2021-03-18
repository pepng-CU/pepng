#include "material.hpp"

Material::Material(GLuint shaderProgram, std::shared_ptr<Texture> texture) : 
    shaderProgram(shaderProgram),
    texture(texture)
{}

Material::Material(const Material& material) : 
    shaderProgram(material.shaderProgram),
    texture(material.texture)
{}

std::shared_ptr<Material> Material::makeMaterial(GLuint shaderProgram, std::shared_ptr<Texture> texture) {
    std::shared_ptr<Material> material(new Material(shaderProgram, texture));

    return material;
}

Material* Material::cloneImplementation() {
    return new Material(*this);
}

GLuint Material::getShaderProgram() {
    return this->shaderProgram;
}

namespace pepng {
    std::shared_ptr<Material> makeMaterial(GLuint shaderProgram, std::shared_ptr<Texture> texture) {
        return Material::makeMaterial(shaderProgram, texture);
    }
};