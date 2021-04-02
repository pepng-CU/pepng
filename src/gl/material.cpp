#include "material.hpp"

Material::Material(GLuint shaderProgram, std::shared_ptr<Texture> texture) : 
    __shader_program(shaderProgram),
    texture(texture)
{}

Material::Material(const Material& material) : 
    __shader_program(material.__shader_program),
    texture(material.texture)
{}

std::shared_ptr<Material> Material::make_material(GLuint shaderProgram, std::shared_ptr<Texture> texture) {
    std::shared_ptr<Material> material(new Material(shaderProgram, texture));

    return material;
}

Material* Material::clone_implementation() {
    return new Material(*this);
}

namespace pepng {
    std::shared_ptr<Material> make_material(GLuint shaderProgram, std::shared_ptr<Texture> texture) {
        return Material::make_material(shaderProgram, texture);
    }
};