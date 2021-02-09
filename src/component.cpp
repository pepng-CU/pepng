#include "component.hpp"

void ComponentManager::attach(std::shared_ptr<Component> component) {
    this->components.push_back(component);
}
        
FPSComponent::FPSComponent(float panSpeed, float rotationSpeed) : 
    panSpeed(panSpeed),
    rotationSpeed(rotationSpeed)
{}

void FPSComponent::update(std::shared_ptr<Transform> parent) {
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

MovementComponent::MovementComponent(float positionSpeed, float rotationSpeed) :
    positionSpeed(positionSpeed),
    rotationSpeed(rotationSpeed)
{}

void MovementComponent::update(std::shared_ptr<Transform> parent) {
    auto input = Input::get();

    auto deltaPosition = glm::vec4(input->getAxis("horizontal"), 0.0f, input->getAxis("vertical"), 0.0f);

    if (glm::length(deltaPosition) > 0.0f) {
        parent->position += glm::vec3(parent->getRotationMatrix() * deltaPosition) * this->positionSpeed;
    }

    auto deltaRotation = glm::vec3(input->getAxis("yaw"), 0.0f, input->getAxis("pitch"));

    if (glm::length(deltaRotation) > 0.0f) {
        parent->deltaRotate(glm::normalize(deltaRotation) * this->rotationSpeed);
    }
}

RenderModeComponent::RenderModeComponent(std::shared_ptr<GLenum> renderMode) : renderMode(renderMode) {}

void RenderModeComponent::update(std::shared_ptr<Transform> parent) {
    auto input = Input::get();

    if(input->getButtonDown("triangles")) {
        *this->renderMode = GL_TRIANGLES;
    } else if(input->getButtonDown("points")) {
        *this->renderMode = GL_POINTS;
    } else if(input->getButtonDown("lines")) {
        *this->renderMode = GL_LINES;
    }
}