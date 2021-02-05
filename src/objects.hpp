#pragma once

#include "object.hpp"

class Axes : public Object {
    public:
        Axes();

        Axes(Transform transform, GLuint shaderProgram);

        virtual void render(std::shared_ptr<Camera> camera, GLenum mode = GL_TRIANGLES, glm::mat4 parentMatrix = glm::mat4(1.0f)) override;
};

class Grid : public Object {
    public:
        Grid();

        Grid(Transform transform, GLuint shaderProgram, int count = 10, glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 0.5f));
};