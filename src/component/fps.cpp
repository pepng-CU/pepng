#include "fps.hpp"
#include "../io/io.hpp"
#include "transform.hpp"

FPS::FPS(float panSpeed, float rotationSpeed) : 
    Component("FPS"),
    panSpeed(panSpeed),
    rotationSpeed(rotationSpeed)
{}

std::shared_ptr<FPS> FPS::makeFPS(float panSpeed, float rotationSpeed) {
    std::shared_ptr<FPS> fps(new FPS(panSpeed, rotationSpeed));

    return fps;
}

std::shared_ptr<FPS> pepng::makeFPS(float panSpeed, float rotationSpeed) {
    return FPS::makeFPS(panSpeed, rotationSpeed);
}

void FPS::update(std::shared_ptr<WithComponents> parent) {
    if(!this->isActive) {
        return;
    }

    auto transform = parent->getComponent<Transform>();

    auto input = Input::get();
    auto mouseDelta = glm::vec2(input->getAxis("mouseX"), input->getAxis("mouseY"));

    if (input->getButtonDown("pan")) {
        transform->position += transform->getUp() * mouseDelta.y * this->panSpeed + transform->getRight() * mouseDelta.x * this->panSpeed;
    }

    if(input->getButtonDown("rotate")) {
        transform->deltaRotate(glm::vec3(mouseDelta.y, mouseDelta.x, 0.0f) * this->rotationSpeed);
    }

    if(input->getButtonDown("zoom")) {
        transform->position += transform->getForward() * input->getAxis("mouseY") * this->panSpeed;
    }
}

void FPS::imgui() {
    Component::imgui();

    ImGui::InputFloat("Pan Speed", &this->panSpeed);
    ImGui::InputFloat("Rotation Speed", &this->rotationSpeed);
}