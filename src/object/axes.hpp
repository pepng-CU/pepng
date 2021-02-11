#pragma once

#include "object.hpp"

namespace pepng {
    std::shared_ptr<Object> makeAxes(std::shared_ptr<Transform> transform, GLuint shaderProgram);
}