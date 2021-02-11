#include "io.hpp"

DeviceUnit::DeviceUnit(std::string name) : name(name), value(0.0f), device(nullptr) {}

float DeviceUnit::getValue() {
    return this->value;
}

Button::Button(std::string name, int buttonId, float strength) : DeviceUnit(name), buttonId(buttonId), strength(strength) {}

std::vector<std::shared_ptr<Button>> Button::buttons = std::vector<std::shared_ptr<Button>>();

std::shared_ptr<Button> Button::makeButton(std::string name, int buttonId, float strength) {
    std::shared_ptr<Button> button (new Button(name, buttonId, strength));

    buttons.push_back(button);

    return button;
}

void Button::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_REPEAT) {
        return;
    }

    for(auto button : buttons) {
        if(button->buttonId == key) {
            button->value = (float) (action == GLFW_PRESS) * button->strength;
        }
    }
}

void Button::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    keyboardCallback(window, button, -1, action, mods);
}

Axis::Axis(std::string name, AxisType axisType) : DeviceUnit(name), axisType(axisType) {}

glm::vec2 Axis::cursorPosition = glm::vec2(-1.0f);
std::vector<std::shared_ptr<Axis>> Axis::axes = std::vector<std::shared_ptr<Axis>>();

std::shared_ptr<Axis> Axis::makeAxis(std::string name, AxisType axisType) {
    std::shared_ptr<Axis> axis (new Axis(name, axisType));

    axes.push_back(axis);

    return axis;
}

void Axis::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    for(auto axis : axes) {
        switch(axis->axisType) {
            case AxisType::THIRD:
                axis->value = yoffset;
                break;
            case AxisType::FORTH:
                axis->value = xoffset;
                break;
        }
    }
}

void Axis::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    glm::vec2 newCursorPosition = glm::vec2(xpos, ypos);

    if(cursorPosition.x == -1) {
        cursorPosition = newCursorPosition;

        return;
    }

    auto delta = newCursorPosition - cursorPosition;

    for(auto axis : axes) {
        switch(axis->axisType) {
            case AxisType::FIRST:
                axis->value = delta.y;
                break;
            case AxisType::SECOND:
                axis->value = delta.x;
                break;
        }
    }

    cursorPosition = newCursorPosition;
}

Device::Device(DeviceType deviceType) : deviceType(deviceType) {}

std::shared_ptr<Device> Device::makeDevice(DeviceType deviceType) {
    std::shared_ptr<Device> device (new Device(deviceType));

    return device;
}

std::shared_ptr<Device> Device::attachUnit(std::shared_ptr<DeviceUnit> unit) {
    this->units.push_back(unit);

    unit->device = shared_from_this();

    return shared_from_this();
}

float Device::getAxis(std::string name) {
    float total = 0.0f;

    for(auto unit : this->units) {
        if(name == unit->name) {
            total += unit->getValue();
        }
    }

    return total;
}

bool Device::getButtonDown(std::string name) {
    float value = this->getAxis(name);

    return std::abs(value) > 0.5f;
}

Input::Input(GLFWwindow* window) : window(window) {}

std::vector<std::shared_ptr<Input>> Input::inputs = std::vector<std::shared_ptr<Input>>();

std::shared_ptr<Input> Input::makeInput(GLFWwindow* window) {
    std::shared_ptr<Input> input (new Input(window));

    inputs.push_back(input);

    glfwSetMouseButtonCallback(window, Button::mouseButtonCallback);
    glfwSetKeyCallback(window, Button::keyboardCallback);
    glfwSetCursorPosCallback(window, Axis::cursorPositionCallback);
    glfwSetScrollCallback(window, Axis::scrollCallback);
    
    return input;
}

std::shared_ptr<Input> Input::attachDevice(std::shared_ptr<Device> device) {
    this->devices.push_back(device);

    device->input = shared_from_this();

    return shared_from_this();
}

float Input::getAxis(std::string name) {
    float total = 0.0f;

    for(auto device : this->devices) {
        total += device->getAxis(name);
    }

    return total;
}

bool Input::getButtonDown(std::string name) {
    float value = this->getAxis(name);

    return std::abs(value) > 0.5f;
}

GLFWwindow* Input::getWindow() {
    return this->window;
}

std::shared_ptr<Input> Input::get() {
    return inputs.at(0);
}

namespace pepng {
    std::shared_ptr<Input> makeInput(GLFWwindow* window) {
        return Input::makeInput(window);
    }

    std::shared_ptr<Device> makeDevice(DeviceType deviceType) {
        return Device::makeDevice(deviceType);
    }

    std::shared_ptr<Axis> makeAxis(std::string name, AxisType axisType) {
        return Axis::makeAxis(name, axisType);
    }

    std::shared_ptr<Button> makeButton(std::string name, int buttonId, float strength) {
        return Button::makeButton(name, buttonId, strength);
    }
}