#include "fps.hpp"

#include "../io/io.hpp"
#include "transform.hpp"

FPS::FPS(float panSpeed, float rotationSpeed) : 
    Component("FPS"),
    panSpeed(panSpeed),
    rotationSpeed(rotationSpeed)
{}

FPS::FPS(const FPS& fps) : 
    Component(fps),
    panSpeed(fps.panSpeed),
    rotationSpeed(fps.rotationSpeed)
{}

std::shared_ptr<FPS> FPS::makeFPS(float panSpeed, float rotationSpeed) {
    std::shared_ptr<FPS> fps(new FPS(panSpeed, rotationSpeed));

    return fps;
}

std::shared_ptr<FPS> pepng::makeFPS(float panSpeed, float rotationSpeed) {
    return FPS::makeFPS(panSpeed, rotationSpeed);
}

FPS* FPS::cloneImplementation() {
    return new FPS(*this);
}

void FPS::update(std::shared_ptr<WithComponents> parent) {
    if(!this->isActive) {
        return;
    }

    auto transform = parent->getComponent<Transform>();

    auto input = Input::get();
    auto mouseDelta = glm::vec2(input->getAxis("mouseX"), input->getAxis("mouseY"));

    if (input->getButton("pan")) {
        transform->position += transform->getUp() * mouseDelta.y * this->panSpeed + transform->getRight() * mouseDelta.x * this->panSpeed;
    }

    auto rotation = glm::vec3(mouseDelta.y, mouseDelta.x, 0.0f);

    if(input->getButton("rotate") && glm::length(rotation) > 0.25f) {
        transform->deltaRotate(rotation * this->rotationSpeed);
    }

    transform->position -= transform->getForward() * input->getAxis("zoom") * this->panSpeed;
}

void FPS::imgui() {
    Component::imgui();

    ImGui::InputFloat("Pan Speed", &this->panSpeed);
    ImGui::InputFloat("Rotation Speed", &this->rotationSpeed);
}