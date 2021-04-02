#define GLEW_STATIC 1
#define STB_IMAGE_IMPLEMENTATION

#pragma once

#include <iostream>
#include <algorithm>
#include <filesystem>
#include <stdlib.h>
#include <time.h>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "../io/io.hpp"
#include "../object/object.hpp"

namespace pepng {
    /**
     * Initializes pepng.
     */
    bool init(const char *title);

    /**
     * Renders and update pepng until program close/failure.
     * 
     * @return Exit code.
     */
    int update();

    /**
     * Attaches device to current pepng instance.
     */
    void attach_device(std::shared_ptr<Device> device);

    /**
     * Inserts object to world.
     */
    void instantiate(std::shared_ptr<Object> object);

    /**
     * Accessor for window.
     */
    GLFWwindow *window();

    /**
     * Accessor for input.
     */
    std::shared_ptr<Input> input();

    /**
     * Accessor for windowX.
     */
    float windowX();

    /**
     * Accessor for windowY.
     */
    float windowY();
}