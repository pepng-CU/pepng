#pragma once

#include "object.hpp"

class Axes : public Object {
    public:
        Axes(Transform transform, GLuint shaderProgram);
};

class Grid : public Object {
    public:
        Grid(Transform transform, GLuint shaderProgram, int count = 10, glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 0.5f));
};