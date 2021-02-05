#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "transform.hpp"

class Viewport {
    public:
        glm::vec2 position;
        glm::vec2 scale;

        /**
         * Attempts to render the viewport.
         * Returns if the viewport is visible or not.
         */
        bool render();
};

class Camera {
    public:
        Transform transform;
        Viewport viewport;
        glm::mat4 projection;

        Camera(Transform transform, Viewport viewport, glm::mat4 projection);

        void render(GLuint shaderProgram);
};