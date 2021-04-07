#define GLEW_STATIC 1
#define STB_IMAGE_IMPLEMENTATION

#pragma once

#include <iostream>
#include <algorithm>
#include <filesystem>
#include <stdlib.h>
#include <time.h>
#include <sstream>

#ifdef EMSCRIPTEN
    #include <emscripten.h>
    #include <emscripten/emscripten.h>
    #define GLFW_INCLUDE_ES3
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef IMGUI
    #include <imgui.h>
    #include <imgui_impl_opengl3.h>
    #include <imgui_impl_glfw.h>
#endif

#include "../io/io.hpp"
#include "../object/object.hpp"

namespace pepng {
    /**
     * Initializes pepng.
     */
    bool init(const char *title, float width, float height);

    /**
     * Renders and update pepng until program close/failure.
     * 
     * Calls pepng::do_frame if no update method is provided.
     * 
     * @return Exit code.
     */
    int update(std::function<void()> do_frame = nullptr);

    /**
     * Sets the object shader.
     */
    void set_object_shader(GLuint shaderProgram);

    /**
     * Sets the shadow shader.
     */
    void set_shadow_shader(GLuint shaderProgram);

    /**
     * Sets the background color. Needs to be called before the update.
     */
    void set_background_color(glm::vec3 color);

    /**
     * Imports image file and sets missing texture.
     * 
     * This should be called before all other texture bindings.
     */
    void set_missing_texture(const std::filesystem::path& filePath);

    /**
     * This sets the window icon.
     */
    void set_window_icon(const std::filesystem::path& filePath);

    /**
     * Attaches device to current pepng instance.
     */
    void attach_device(std::shared_ptr<Device> device);

    /**
     * Creates instance of object in world.
     */
    void instantiate(std::shared_ptr<Object> object);

    /**
     * Accessor for window.
     */
    GLFWwindow *window();

    /**
     * Accessor for world.
     */
    std::vector<std::shared_ptr<Object>> world();

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

    namespace extra {
        /**
         * Method called in frame loop for updating objects.
         */
        void update_objects();

        /**
         * Method called in frame loop for rendering shadows.
         */
        void render_shadows();

        /**
         * Method called in frame loop for rendering objects.
         */
        void render_objects();

        /**
         * Method called in frame loop for rendering IMGUI.
         */
        void render_imgui();

        /**
         * Method called in frame loop for updating GLFW.
         */
        void update_glfw();
    }
}