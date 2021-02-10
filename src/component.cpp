#include "component.hpp"

Component::Component(std::string name) : 
    name(name),
    isActive(true)
{}

std::string Component::getName() {
    return this->name;
}

void Component::imgui() {
    ImGui::Checkbox("Active", &this->isActive);
}

void ComponentManager::attach(std::shared_ptr<Component> component) {
    this->components.push_back(component);
}

void ComponentManager::imgui() {
    for(auto component : this->getComponents()) {
        if(ImGui::CollapsingHeader(component->getName().c_str())) {
            component->imgui();
        }
    }
}

FPSComponent::FPSComponent(float panSpeed, float rotationSpeed) : 
    Component("FPS"),
    panSpeed(panSpeed),
    rotationSpeed(rotationSpeed)
{}

void FPSComponent::update(std::shared_ptr<Transform> parent) {
    if(!this->isActive) {
        return;
    }

    auto input = Input::get();
    auto mouseDelta = glm::vec2(input->getAxis("mouseX"), input->getAxis("mouseY"));

    if (input->getButtonDown("pan")) {
        parent->position += parent->getUp() * mouseDelta.y * this->panSpeed + parent->getRight() * mouseDelta.x * this->panSpeed;
    }

    if(input->getButtonDown("rotate")) {
        parent->deltaRotate(glm::vec3(mouseDelta.y, mouseDelta.x, 0.0f) * this->rotationSpeed);
    }

    if(input->getButtonDown("zoom")) {
        parent->position += parent->getForward() * input->getAxis("mouseY") * this->panSpeed;
    }
}

void FPSComponent::imgui() {
    Component::imgui();

    ImGui::InputFloat("Pan Speed", &this->panSpeed);
    ImGui::InputFloat("Rotation Speed", &this->rotationSpeed);
}

MovementComponent::MovementComponent(float positionSpeed, float rotationSpeed) :
    Component("Movement"),
    positionSpeed(positionSpeed),
    rotationSpeed(rotationSpeed)
{}

void MovementComponent::update(std::shared_ptr<Transform> parent) {
    if(!this->isActive) {
        return;
    }

    auto input = Input::get();

    auto deltaPosition = glm::vec4(input->getAxis("horizontal"), 0.0f, input->getAxis("vertical"), 0.0f);

    if (glm::length(deltaPosition) > 0.0f) {
        parent->position += glm::vec3(parent->getRotationMatrix() * deltaPosition) * this->positionSpeed;
    }

    auto deltaRotation = glm::vec3(input->getAxis("yaw"), 0.0f, input->getAxis("pitch"));

    if (glm::length(deltaRotation) > 0.0f) {
        parent->deltaRotate(deltaRotation * this->rotationSpeed);
    }

    if(input->getButtonDown("recenter")) {
        parent->position = glm::vec3(0.0f);
        parent->setRotation(glm::vec3(0.0f));
    }
}

void MovementComponent::imgui() {
    Component::imgui();
    
    ImGui::InputFloat("Position Speed", &this->positionSpeed);
    ImGui::InputFloat("Rotation Speed", &this->rotationSpeed);
}