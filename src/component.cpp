#include "component.hpp"
        
FPSComponent::FPSComponent(std::shared_ptr<Transform> object) : object(object), isPanning(false), isRotating(false) {}

void FPSComponent::mouseButtonCallback(GLFWwindow* window, int button, int action) {
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        this->isPanning = action == GLFW_PRESS;
    } 
    
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        this->isRotating = action == GLFW_PRESS;
    }
}

void FPSComponent::scrollCallback(GLFWwindow* window, glm::vec2 delta) {
    this->object->position -= this->object->getForward() * delta.y;
}

void FPSComponent::cursorPositionCallback(GLFWwindow* window, glm::vec2 delta) {
    if(this->isPanning) {
        this->object->position += this->object->getUp() * delta.y / 10.0f + this->object->getRight() * delta.x / 10.0f;
    } 

    if(this->isRotating) {
        this->object->deltaRotate(glm::vec3(delta.y, delta.x, 0.0f));
    }
}

MovementComponent::MovementComponent(std::shared_ptr<Transform> object) : object(object), deltaPosition(glm::vec3(0.0f)), deltaRotation(glm::vec3(0.0f)) {}

glm::vec3 MovementComponent::getDeltaPosition(int key) {
    switch(key) {
        case GLFW_KEY_W:
            return glm::vec3(0.0f, 0.0f, 1.0f);
        case GLFW_KEY_S:
            return glm::vec3(0.0f, 0.0f, -1.0f);
        case GLFW_KEY_A:
            return glm::vec3(1.0f, 0.0f, 0.0f);
        case GLFW_KEY_D:
            return glm::vec3(-1.0f, 0.0f, 0.0f);
        default:
            return glm::vec3(0.0f);
    }
}

glm::vec3 MovementComponent::getDeltaRotation(int key) {
    switch(key) {
        case GLFW_KEY_LEFT:
            return glm::vec3(0.0f, -1.0f, 0.0f);
        case GLFW_KEY_RIGHT:
            return glm::vec3(0.0f, 1.0f, 0.0f);
        case GLFW_KEY_DOWN:
            return glm::vec3(-1.0f, 0.0f, 0.0f);
        case GLFW_KEY_UP:
            return glm::vec3(1.0f, 0.0f, 0.0f);
        default:
            return glm::vec3(0.0f);
    }
}

bool MovementComponent::hasDeltaPosition(int key) {
    return glm::length(this->getDeltaPosition(key)) > 0.5f;
}

bool MovementComponent::hasDeltaRotation(int key) {
    return glm::length(this->getDeltaRotation(key)) > 0.5f;
}

void MovementComponent::keyboardCallback(GLFWwindow* window, int key, int action) {
    if(action == GLFW_PRESS) {
        if(key == GLFW_KEY_HOME) {
            this->object->position = glm::vec3(0.0f);
            this->object->setRotation(glm::vec3(0.0f));
        }

        this->deltaPosition += this->getDeltaPosition(key);
        this->deltaRotation += this->getDeltaRotation(key);
    } else if (action == GLFW_RELEASE) {
        this->deltaPosition -= this->getDeltaPosition(key);
        this->deltaRotation -= this->getDeltaRotation(key);
    }
}

void MovementComponent::update() {
    this->object->position += glm::vec3(this->object->getRotationMatrix() * glm::vec4(this->deltaPosition, 0.0f));
    this->object->deltaRotate(this->deltaRotation);
}

ObjectManagerComponent::ObjectManagerComponent(std::vector<std::shared_ptr<Component>> components) : components(components), objectIndex(0) {}

void ObjectManagerComponent::update() {
    for(auto component : this->components) {
        component->update();
    }
}

void ObjectManagerComponent::keyboardCallback(GLFWwindow* window, int key, int action) {
    for(int i = GLFW_KEY_0; i <= std::min(GLFW_KEY_9, (int)(GLFW_KEY_0 + this->components.size())); i++) {
        if(glfwGetKey(window, i) == GLFW_PRESS) {
            this->objectIndex = i - GLFW_KEY_0;
            break;
        }
    }

    this->components.at(this->objectIndex)->keyboardCallback(window, key, action);
}