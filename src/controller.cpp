#include "controller.hpp"

Controller* Controller::instance = nullptr;

Controller::Controller(GLFWwindow* window) : window(window) {}

void Controller::attach(std::shared_ptr<Component> component) {
    this->components.push_back(component);
}

void Controller::setInstance(Controller* controller) {
    instance = controller;

    controller->attachCallbacks();
}

void Controller::update() {
    for(auto component : this->components) {
        component->update();
    }
}

void Controller::attachCallbacks() {
    glfwSetMouseButtonCallback(instance->window, this->mouseButtonCallback);
    glfwSetCursorPosCallback(instance->window, this->cursorPositionCallback);
    glfwSetScrollCallback(instance->window, this->scrollCallback);
    glfwSetKeyCallback(instance->window, this->keyboardCallback);
}

void Controller::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    for(auto component : instance->components) {
        component->mouseButtonCallback(button, action);
    }
}

void Controller::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    for(auto component : instance->components) {
        component->keyboardCallback(key, action);
    }
}

void Controller::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    glm::vec2 delta = glm::vec2(xoffset, yoffset);

    for(auto component : instance->components) {
        component->scrollCallback(delta);
    }
}

void Controller::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    glm::vec2 mousePos = glm::vec2(xpos, ypos);
    glm::vec2 delta = mousePos - instance->previousMousePos;

    for(auto component : instance->components) {
        component->cursorPositionCallback(delta);
    }

    instance->previousMousePos = mousePos;
}