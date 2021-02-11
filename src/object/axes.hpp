#pragma once

#include "object.hpp"
#include "../component/transform.hpp"

namespace pepng {
    std::shared_ptr<Object> makeAxes(std::shared_ptr<Transform> transform, GLuint shaderProgram);
}