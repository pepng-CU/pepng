#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>

enum AxisType {
    FIRST, SECOND, THIRD, FORTH
};

enum DeviceType {
    MOUSE, JOYSTICK, KEYBOARD, NONE
};

class Device;
class Input;

class DeviceUnit : public std::enable_shared_from_this<DeviceUnit> {
    public:
        friend Device;
        friend Input;

        virtual float getValue();

    protected:
        float value;
        float strength;
        std::shared_ptr<Device> device;

        DeviceUnit(std::string name, float strength); 

    private:
        std::string name;
};

class Button : public DeviceUnit, public std::enable_shared_from_this<Button> {
    public:
        friend Input;

        static std::shared_ptr<Button> makeButton(std::string name, int buttonId, float strength = 1);

    protected:
        Button(std::string name, int buttonId, float strength = 1);

    private:
        int buttonId;
        float strength;
        static std::vector<std::shared_ptr<Button>> buttons;
        static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
};

class Axis : public DeviceUnit, public std::enable_shared_from_this<Axis> {
    public:
        friend Input;
        
        static std::shared_ptr<Axis> makeAxis(std::string name, AxisType axisType, float strength, bool needsReset);

        virtual float getValue() override;

    protected:
        Axis(std::string name, AxisType axisType, float strength, bool needsReset);

    private:
        AxisType axisType;
        bool needsReset;
        static std::vector<std::shared_ptr<Axis>> axes;
        static glm::vec2 cursorPosition;
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
};

class Device : public std::enable_shared_from_this<Device> {
    public:
        friend Input;

        static std::shared_ptr<Device> makeDevice(DeviceType deviceType);

        std::shared_ptr<Device> attachUnit(std::shared_ptr<DeviceUnit> unit);

        float getAxis(std::string name);

        bool getButtonDown(std::string name);

    private:
        DeviceType deviceType;
        std::shared_ptr<Input> input;
        std::vector<std::shared_ptr<DeviceUnit>> units;
        
        Device(DeviceType deviceType);
};

class Input : public std::enable_shared_from_this<Input> {
    public:
        static std::shared_ptr<Input> makeInput(GLFWwindow* window);

        std::shared_ptr<Input> attachDevice(std::shared_ptr<Device> device);

        float getAxis(std::string name);

        bool getButtonDown(std::string name);

        GLFWwindow* getWindow();

        static std::shared_ptr<Input> get();

    private:
        GLFWwindow* window;

        std::vector<std::shared_ptr<Device>> devices;

        static std::vector<std::shared_ptr<Input>> inputs;

        Input(GLFWwindow* window);
};

namespace pepng {
    std::shared_ptr<Input> makeInput(GLFWwindow* window);
    std::shared_ptr<Device> makeDevice(DeviceType deviceType);
    std::shared_ptr<Axis> makeAxis(std::string name, AxisType axisType, float strength = 1, bool needsReset = false);
    std::shared_ptr<Button> makeButton(std::string name, int buttonId, float strength = 1);
}