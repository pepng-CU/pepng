#include "io.hpp"

DeviceUnit::DeviceUnit(std::string name, float strength) : __name(name), _value(0.0f), _device(nullptr), _strength(strength) {}

float DeviceUnit::value() {
    return this->_value * this->_strength;
}

Button::Button(std::string name, int buttonId, float strength) : DeviceUnit(name, strength), __button_id(buttonId) {}

std::vector<std::shared_ptr<Button>> Button::__buttons = std::vector<std::shared_ptr<Button>>();

std::shared_ptr<Button> Button::make_button(std::string name, int buttonId, float strength) {
    std::shared_ptr<Button> button (new Button(name, buttonId, strength));

    __buttons.push_back(button);

    return button;
}

void Button::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_REPEAT) {
        return;
    }

    for(auto button : __buttons) {
        if(button->__button_id == key) {
            button->_value = (float) (action == GLFW_PRESS);
        }
    }
}

void Button::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    keyboardCallback(window, button, -1, action, mods);
}

Axis::Axis(std::string name, AxisType axisType, float strength, bool needsReset) : 
    DeviceUnit(name, strength), 
    __axis_type(axisType),  
    __needs_reset(needsReset)
{}

glm::vec2 Axis::__cursor_position = glm::vec2(-1.0f);
std::vector<std::shared_ptr<Axis>> Axis::__axes = std::vector<std::shared_ptr<Axis>>();

std::shared_ptr<Axis> Axis::make_axis(std::string name, AxisType axisType, float strength, bool needsReset) {
    std::shared_ptr<Axis> axis (new Axis(name, axisType, strength, needsReset));

    __axes.push_back(axis);

    return axis;
}

void Axis::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    for(auto axis : __axes) {
        switch(axis->__axis_type) {
            case AxisType::THIRD:
                axis->_value = yoffset;
                break;
            case AxisType::FORTH:
                axis->_value = xoffset;
                break;
        }
    }
}

void Axis::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    glm::vec2 newCursorPosition = glm::vec2(xpos, ypos);

    if(__cursor_position.x == -1) {
        __cursor_position = newCursorPosition;

        return;
    }

    auto delta = newCursorPosition - __cursor_position;

    for(auto axis : __axes) {
        switch(axis->__axis_type) {
            case AxisType::FIRST:
                axis->_value = delta.y;
                break;
            case AxisType::SECOND:
                axis->_value = delta.x;
                break;
        }
    }

    __cursor_position = newCursorPosition;
}

float Axis::value() {
    float currentValue = DeviceUnit::value();

    if(this->__needs_reset) {
        this->_value = 0.0f;
    }

    return currentValue;
}

Device::Device(DeviceType deviceType) : __device_type(deviceType) {}

std::shared_ptr<Device> Device::make_device(DeviceType deviceType) {
    std::shared_ptr<Device> device (new Device(deviceType));

    return device;
}

std::shared_ptr<Device> Device::attach_unit(std::shared_ptr<DeviceUnit> unit) {
    this->__units.push_back(unit);

    unit->_device = shared_from_this();

    return shared_from_this();
}

float Device::axis(std::string name) {
    float total = 0.0f;

    for(auto unit : this->__units) {
        if(name == unit->__name) {
            total += unit->value();
        }
    }

    return total;
}

bool Device::button(std::string name) {
    float value = this->axis(name);

    return std::abs(value) > 0.5f;
}

bool Device::button_down(std::string name) {
    for(auto unit : this->__units) {
        if(name == unit->__name && unit->_value != 0.0f) {
            auto value = unit->value();
            unit->_value = 0.0f;

            return std::abs(value) > 0.5f;
        }
    }

    return false;
}

Input::Input(GLFWwindow* window) : __window(window) {}

std::vector<std::shared_ptr<Input>> Input::__inputs = std::vector<std::shared_ptr<Input>>();

std::shared_ptr<Input> Input::make_input(GLFWwindow* window) {
    std::shared_ptr<Input> input(new Input(window));

    __inputs.push_back(input);

    glfwSetMouseButtonCallback(window, Button::mouseButtonCallback);
    glfwSetKeyCallback(window, Button::keyboardCallback);
    glfwSetCursorPosCallback(window, Axis::cursorPositionCallback);
    glfwSetScrollCallback(window, Axis::scrollCallback);
    
    return input;
}

std::shared_ptr<Input> Input::attach_device(std::shared_ptr<Device> device) {
    this->__devices.push_back(device);

    device->__input = shared_from_this();

    return shared_from_this();
}

float Input::axis(std::string name) {
    float total = 0.0f;

    for(auto device : this->__devices) {
        total += device->axis(name);
    }

    return total;
}

bool Input::button(std::string name) {
    float value = this->axis(name);

    return std::abs(value) > 0.5f;
}

bool Input::button_down(std::string name) {
    for(auto device : this->__devices) {
        if(device->button_down(name)) {
            return true;
        }
    }

    return false;
}

GLFWwindow* Input::window() {
    return this->__window;
}

std::shared_ptr<Input> Input::get() {
    return __inputs.at(0);
}

namespace pepng {
    std::shared_ptr<Input> make_input(GLFWwindow* window) {
        return Input::make_input(window);
    }

    std::shared_ptr<Device> make_device(DeviceType deviceType) {
        return Device::make_device(deviceType);
    }

    std::shared_ptr<Axis> make_axis(std::string name, AxisType axisType, float strength, bool needsResets) {
        return Axis::make_axis(name, axisType, strength, needsResets);
    }

    std::shared_ptr<Button> make_button(std::string name, int buttonId, float strength) {
        return Button::make_button(name, buttonId, strength);
    }
}