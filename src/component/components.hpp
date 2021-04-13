#pragma once

/**
 * This is the module hpp that you can import for all components.
 */
#include "camera.hpp"

#include "light.hpp"
#include "pointlight.hpp"
#include "spotlight.hpp"
#include "renderer.hpp"
#include "transform.hpp"
#include "with_components.hpp"

#ifdef EXTRA_COMPONENTS
    #include "extra/dynamic_texture.hpp"
    #include "extra/fps.hpp"
    #include "extra/selector.hpp"
    #include "extra/transformer.hpp"
#endif 