#pragma once

#include "object.hpp"

class Axes : public Object {
    public:
        Axes();

        Axes(Transform transform, GLuint shaderProgram);

        virtual void render(std::shared_ptr<Camera> camera, GLenum mode = GL_TRIANGLES, glm::mat4 parentMatrix = glm::mat4(1.0f));
};

class Grid : public Object {
    public:
        Grid();

        Grid(Transform transform, GLuint shaderProgram, int count = 10);
};