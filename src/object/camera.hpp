#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "object.hpp"
#include "../component/camera.hpp"

namespace pepng {
    std::shared_ptr<Object> makeCameraObj(std::shared_ptr<CameraTransform> transform, std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection);
};