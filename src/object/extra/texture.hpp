#pragma once

#include "../object.hpp"
#include "../../component/transform.hpp"

namespace pepng {
    /**
     * Creates a Texture object.
     */
    std::shared_ptr<Object> make_texture_object(std::shared_ptr<Transform> transform, GLuint shaderProgram);
}