#include "movement.hpp"
#include "transform.hpp"

MovementComponent::MovementComponent(float positionSpeed, float rotationSpeed) :
    Component("Movement"),
    positionSpeed(positionSpeed),
    rotationSpeed(rotationSpeed)
{}

void MovementComponent::update(std::shared_ptr<WithComponents> parent) {
    if(!this->isActive) {
        return;
    }

    auto transform = parent->getComponent<Transform>();

    auto input = Input::get();

    auto deltaPosition = glm::vec4(input->getAxis("horizontal"), 0.0f, input->getAxis("vertical"), 0.0f);

    if (glm::length(deltaPosition) > 0.0f) {
        transform->position += glm::vec3(transform->getRotationMatrix() * deltaPosition) * this->positionSpeed;
    }

    auto deltaRotation = glm::vec3(input->getAxis("yaw"), 0.0f, input->getAxis("pitch"));

    if (glm::length(deltaRotation) > 0.0f) {
        transform->deltaRotate(deltaRotation * this->rotationSpeed);
    }

    if(input->getButtonDown("recenter")) {
        transform->position = glm::vec3(0.0f);
        transform->setRotation(glm::vec3(0.0f));
    }
}

void MovementComponent::imgui() {
    Component::imgui();
    
    ImGui::InputFloat("Position Speed", &this->positionSpeed);
    ImGui::InputFloat("Rotation Speed", &this->rotationSpeed);
}