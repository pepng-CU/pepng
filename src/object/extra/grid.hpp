#pragma once

#include "../object.hpp"

namespace pepng {
    /**
     * Creates a Grid object.
     */
    std::shared_ptr<Object> make_grid(std::shared_ptr<Transform> transform, GLuint shaderProgram, int count = 10, glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 0.5f));
};