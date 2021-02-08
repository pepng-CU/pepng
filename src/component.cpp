#include "component.hpp"
        
FPSComponent::FPSComponent(std::shared_ptr<Transform> object) : object(object), isPanning(false), isRotating(false) {}

void FPSComponent::mouseButtonCallback(int button, int action) {
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        this->isPanning = action == GLFW_PRESS;
    } 
    
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        this->isRotating = action == GLFW_PRESS;
    }
}

void FPSComponent::keyboardCallback(int key, int action) {
}

void FPSComponent::scrollCallback(glm::vec2 delta) {
    this->object->position -= this->object->getForward() * delta.y;
}

void FPSComponent::cursorPositionCallback(glm::vec2 delta) {
    if(this->isPanning) {
        this->object->position += this->object->getUp() * delta.y / 10.0f + this->object->getRight() * delta.x / 10.0f;
    } 

    if(this->isRotating) {
        this->object->deltaRotate(glm::vec3(delta.y, delta.x, 0.0f));
    }
}

void FPSComponent::update() {
}