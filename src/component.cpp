#include "component.hpp"
        
FPSComponent::FPSComponent(std::shared_ptr<Transform> object, float panSpeed, float rotationSpeed) : 
    object(object), 
    isPanning(false), 
    isRotating(false),
    panSpeed(panSpeed),
    rotationSpeed(rotationSpeed)
{}

void FPSComponent::mouseButtonCallback(GLFWwindow* window, int button, int action) {
    if(button == GLFW_MOUSE_BUTTON_MIDDLE) {
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
        this->object->position += this->object->getUp() * delta.y * this->panSpeed + this->object->getRight() * delta.x * this->panSpeed;
    } 

    if(this->isRotating) {
        this->object->deltaRotate(glm::vec3(delta.y, delta.x, 0.0f) * this->rotationSpeed);
    }
}

MovementComponent::MovementComponent(std::shared_ptr<Transform> object, float positionSpeed, float rotationSpeed) :
    object(object), 
    deltaPosition(glm::vec3(0.0f)), 
    deltaRotation(glm::vec3(0.0f)),
    positionSpeed(positionSpeed),
    rotationSpeed(rotationSpeed)
{}

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
    if(glm::length(this->deltaPosition) > 0.0f) {
        this->object->position += glm::normalize(glm::vec3(this->object->getRotationMatrix() * glm::vec4(this->deltaPosition, 0.0f))) * this->positionSpeed;
    }
    
    if(glm::length(this->deltaRotation) > 0.0f) {
        this->object->deltaRotate(glm::normalize(this->deltaRotation) * this->rotationSpeed);
    }
}

std::shared_ptr<Transform> MovementComponent::getTransform() {
    return this->object;
}

ObjectManagerComponent::ObjectManagerComponent(std::vector<std::shared_ptr<MovementComponent>> components) : components(components), componentIndex(0) {}

std::shared_ptr<MovementComponent> ObjectManagerComponent::getCurrentComponent() {
    return this->components.at(this->componentIndex);
}

void ObjectManagerComponent::update() {
    for(auto component : this->components) {
        component->update();
    }
}

void ObjectManagerComponent::keyboardCallback(GLFWwindow* window, int key, int action) {
    for(int i = GLFW_KEY_0; i <= std::min(GLFW_KEY_9, (int)(GLFW_KEY_0 + this->components.size() - 1)); i++) {
        if(glfwGetKey(window, i) == GLFW_PRESS) {
            this->componentIndex = i - GLFW_KEY_0;
            break;
        }
    }

    this->components.at(this->componentIndex)->keyboardCallback(window, key, action);
}

void EscapeComponent::keyboardCallback(GLFWwindow* window, int key, int action) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

RenderModeComponent::RenderModeComponent(std::shared_ptr<GLenum> renderMode) : renderMode(renderMode) {}

GLenum RenderModeComponent::getRenderMode(int key) {
    switch(key) {
        case GLFW_KEY_T:
            return GL_TRIANGLES;
        case GLFW_KEY_P:
            return GL_POINTS;
        case GLFW_KEY_L:
            return GL_LINES;
        default:
            return -1;
    }
}

bool RenderModeComponent::hasRenderMode(int key) {
    return this->getRenderMode(key) != -1;
}

void RenderModeComponent::keyboardCallback(GLFWwindow* window, int key, int action) {
    if (action != GLFW_PRESS) {
        return;
    }

    if(this->hasRenderMode(key)) {
        *this->renderMode = this->getRenderMode(key);
    }
}