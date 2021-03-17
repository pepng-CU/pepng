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

/**
 * An abstract Device unit (Button, Axis, etc).
 */
class DeviceUnit : public std::enable_shared_from_this<DeviceUnit> {
    public:
        friend Device;
        friend Input;

        /**
         * Accessor for the device value.
         */
        virtual float getValue();

    protected:
        /**
         * The current device unit value.
         */
        float value;
        /**
         * The strength multiplier for the device.
         */
        float strength;
        /**
         * Parent device.
         */
        std::shared_ptr<Device> device;

        DeviceUnit(std::string name, float strength); 

    private:
        /**
         * The label for device unit.
         */
        std::string name;
};

/**
 * On/off device unit.
 */
class Button : public DeviceUnit, public std::enable_shared_from_this<Button> {
    public:
        friend Input;

        /**
         * Shared_ptr constructor for Button.
         */
        static std::shared_ptr<Button> makeButton(std::string name, int buttonId, float strength = 1);

    protected:
        Button(std::string name, int buttonId, float strength = 1);

    private:
        /**
         * The GLFW button value.
         */
        int buttonId;
        /**
         * Instantiation of Buttons.
         */
        static std::vector<std::shared_ptr<Button>> buttons;
        /**
         * Keyboard button callback (which uses buttons vector).
         */
        static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        /**
         * Mouse button callback (which uses buttons vector).
         */
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
};

/**
 * Float device unit.
 */
class Axis : public DeviceUnit, public std::enable_shared_from_this<Axis> {
    public:
        friend Input;
        
        static std::shared_ptr<Axis> makeAxis(std::string name, AxisType axisType, float strength, bool needsReset);

        virtual float getValue() override;

    protected:
        Axis(std::string name, AxisType axisType, float strength, bool needsReset);

    private:
        /**
         * The GLFW axis type.
         */
        AxisType axisType;
        /**
         * Method to reset value when reset.
         */
        bool needsReset;
        /**
         * Instantiation of Axes.
         */
        static std::vector<std::shared_ptr<Axis>> axes;
        /**
         * The static XY position of the cursor.
         */
        static glm::vec2 cursorPosition;
        /**
         * Scroll callback (uses axes).
         */
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        /**
         * Cursor callback (uses cursorPosition).
         */
        static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
};

/**
 * Device for collection of Device unit.
 */
class Device : public std::enable_shared_from_this<Device> {
    public:
        friend Input;

        /**
         * Shared_ptr constructor of Device.
         */
        static std::shared_ptr<Device> makeDevice(DeviceType deviceType);

        /**
         * Attaches a device unit to the device.
         */
        std::shared_ptr<Device> attachUnit(std::shared_ptr<DeviceUnit> unit);

        /**
         * Get an Axis device unit value by name.
         */
        float getAxis(std::string name);

        /**
         * Get a Button device unit value by name.
         */
        bool getButton(std::string name);

        /**
         * Get a Button device unit value by name (reset to zero once called).
         */
        bool getButtonDown(std::string name);

    private:
        /**
         * The device type using the DeviceType enum.
         */
        DeviceType deviceType;
        /**
         * The parent input device.
         */
        std::shared_ptr<Input> input;
        /**
         * List of attached device unit.
         */
        std::vector<std::shared_ptr<DeviceUnit>> units;
        
        Device(DeviceType deviceType);
};

/**
 * Global input manager (which should be attached to window).
 */
class Input : public std::enable_shared_from_this<Input> {
    public:
        /**
         * Shared_ptr constructor for Input.
         */
        static std::shared_ptr<Input> makeInput(GLFWwindow* window);

        /**
         * Attaches a device to the input.
         */
        std::shared_ptr<Input> attachDevice(std::shared_ptr<Device> device);

        /**
         * Gets an Axis value from all attached devices by name.
         */
        float getAxis(std::string name);

        /**
         * Gets a Button value from all attached devices by name.
         */
        bool getButton(std::string name);

        /**
         * Get a Button device unit value by name (reset to zero once called).
         */
        bool getButtonDown(std::string name);

        /**
         * Gets the window that this input is attached to.
         */
        GLFWwindow* getWindow();

        /**
         * Gets the current input manager.
         * 
         * TODO: This currently assumes that there is only one window (which shouldn't really be an issue).
         */
        static std::shared_ptr<Input> get();

    private:
        /**
         * The window that this input manager is attached to.
         */
        GLFWwindow* window;

        /**
         * List of attached devices.
         */
        std::vector<std::shared_ptr<Device>> devices;

        /**
         * List of inputs.
         */
        static std::vector<std::shared_ptr<Input>> inputs;

        Input(GLFWwindow* window);
};

namespace pepng {
    std::shared_ptr<Input> makeInput(GLFWwindow* window);
    std::shared_ptr<Device> makeDevice(DeviceType deviceType);
    std::shared_ptr<Axis> makeAxis(std::string name, AxisType axisType, float strength = 1, bool needsReset = false);
    std::shared_ptr<Button> makeButton(std::string name, int buttonId, float strength = 1);
}