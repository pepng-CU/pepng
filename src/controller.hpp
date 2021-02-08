#pragma once

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "component.hpp"

class Controller {
    public:
        static Controller* instance;

        Controller(GLFWwindow* window);

        void attach(std::shared_ptr<Component> component);
        void attachCallbacks();
        void update();

        static void setInstance(Controller* controller);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    private:
        GLFWwindow* window;
        std::vector<std::shared_ptr<Component>> components;
        glm::vec2 previousMousePos;
};