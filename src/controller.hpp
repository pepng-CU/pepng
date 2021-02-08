#pragma once

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>

#include "component.hpp"

class Controller {
    private:
    std::vector<std::shared_ptr<Component>> components;

    public:
    Controller();
    void attach(std::shared_ptr<Component> component);
    void attachCallbacks();
    void callBackHandler(GLFWwindow *window, int, int, int, int);
};