#include "renderer.hpp"
#include "../io/io.hpp"
#include "transform.hpp"
#include "../object/camera.hpp"

DynamicTexture::DynamicTexture(int startTextureIndex, int endTextureIndex) :
    Component("DynamicTexture"),
    startTextureIndex(startTextureIndex), 
    endTextureIndex(endTextureIndex),
    currentIndex(startTextureIndex - 1),
    count(MAX_COUNT)
{}

DynamicTexture::DynamicTexture(const DynamicTexture& dynamicTexture) :
    Component(dynamicTexture),
    startTextureIndex(dynamicTexture.startTextureIndex), 
    endTextureIndex(dynamicTexture.endTextureIndex),
    currentIndex(dynamicTexture.currentIndex),
    count(dynamicTexture.count)
{}

std::shared_ptr<DynamicTexture> DynamicTexture::makeDynamicTexture(int startTextureIndex, int endTextureIndex) {
    std::shared_ptr<DynamicTexture> dynamicTexture(new DynamicTexture(startTextureIndex, endTextureIndex));

    return dynamicTexture;
}

std::shared_ptr<DynamicTexture> pepng::makeDynamicTexture(int startTextureIndex, int endTextureIndex) {
    return DynamicTexture::makeDynamicTexture(startTextureIndex, endTextureIndex);
}

DynamicTexture* DynamicTexture::cloneImplementation() {
    return new DynamicTexture(*this);
}

void DynamicTexture::update(std::shared_ptr<WithComponents> parent) {
    if(this->count++ < MAX_COUNT) return;

    this->count = 0;

    std::shared_ptr<Renderer> renderer;

    try {
        renderer = parent->getComponent<Renderer>();
    } catch(...) {
        throw std::runtime_error("DynamicTexture requires a Renderer component.");
    }

    if(++this->currentIndex > this->endTextureIndex) {
        this->currentIndex = this->startTextureIndex;
    }

    renderer->material->texture = pepng::makeTexture(this->currentIndex);
}

void DynamicTexture::imgui() {
    Component::imgui();

    ImGui::InputInt("Start", &this->startTextureIndex);
    ImGui::InputInt("End", &this->endTextureIndex);
    ImGui::InputInt("Current", &this->currentIndex);
}