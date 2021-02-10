#pragma once

#include "object.hpp"

class Axes : public Object {
    public:
        Axes(std::shared_ptr<Transform> transform, GLuint shaderProgram);
};