#pragma once

#include "object.hpp"

class Grid : public Object {
    public:
        Grid(std::shared_ptr<Transform> transform, GLuint shaderProgram, int count = 10, glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 0.5f));
};