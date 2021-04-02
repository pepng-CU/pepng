#include "dynamic_texture.hpp"

#include "renderer.hpp"
#include "../io/io.hpp"
#include "transform.hpp"
#include "../object/camera.hpp"

DynamicTexture::DynamicTexture(int startTextureIndex, int endTextureIndex) :
    Component("DynamicTexture"),
    start_texture_index(startTextureIndex), 
    end_texture_index(endTextureIndex),
    current_index(startTextureIndex - 1),
    count(MAX_COUNT)
{}

DynamicTexture::DynamicTexture(const DynamicTexture& dynamicTexture) :
    Component(dynamicTexture),
    start_texture_index(dynamicTexture.start_texture_index), 
    end_texture_index(dynamicTexture.end_texture_index),
    current_index(dynamicTexture.current_index),
    count(dynamicTexture.count)
{}

std::shared_ptr<DynamicTexture> DynamicTexture::make_dynamic_texture(int startTextureIndex, int endTextureIndex) {
    std::shared_ptr<DynamicTexture> dynamicTexture(new DynamicTexture(startTextureIndex, endTextureIndex));

    return dynamicTexture;
}

std::shared_ptr<DynamicTexture> pepng::make_dynamic_texture(int startTextureIndex, int endTextureIndex) {
    return DynamicTexture::make_dynamic_texture(startTextureIndex, endTextureIndex);
}

DynamicTexture* DynamicTexture::clone_implementation() {
    return new DynamicTexture(*this);
}

void DynamicTexture::update(std::shared_ptr<WithComponents> parent) {
    if(this->count++ < MAX_COUNT) return;

    this->count = 0;

    std::shared_ptr<Renderer> renderer;

    try {
        renderer = parent->get_component<Renderer>();
    } catch(...) {
        throw std::runtime_error("DynamicTexture requires a Renderer component.");
    }

    if(++this->current_index > this->end_texture_index) {
        this->current_index = this->start_texture_index;
    }

    renderer->material->texture = pepng::make_texture(this->current_index);
}

void DynamicTexture::imgui() {
    Component::imgui();

    ImGui::InputInt("Start", &this->start_texture_index);
    ImGui::InputInt("End", &this->end_texture_index);
    ImGui::InputInt("Current", &this->current_index);
}