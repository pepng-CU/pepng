#pragma once

#include <memory>

#include <GL/glew.h>
#include <glm/gtx/string_cast.hpp>

#include "../component.hpp"
#include "../renderer.hpp"
#include "../../gl/model.hpp"
#include "../../gl/material.hpp"

/**
 * Component that switches the Renderer's texture every ~10 seconds.
 */
class DynamicTexture : public Component {
    public:
        /**
         * The first texture index.
         */
        int start_texture_index;

        /**
         * The last texture index.
         */
        int end_texture_index;

        /**
         * Current active texture.
         */
        int current_index;

        /**
         * Game cycle counter.
         */
        int count;

        /**
         * Number of ticks for texture to switch.
         */
        static const int MAX_COUNT = 1000;

        /**
         * Shared_ptr constructor for DynamicTexture.
         */
        static std::shared_ptr<DynamicTexture> make_dynamic_texture(int startTextureIndex, int endTextureIndex);

        virtual void update(std::shared_ptr<WithComponents> object) override;
        virtual void imgui() override;

        virtual DynamicTexture* clone_implementation() override;
    private:
        DynamicTexture(int startTextureIndex, int endTextureIndex);
        DynamicTexture(const DynamicTexture& renderer);
};

namespace pepng {
    std::shared_ptr<DynamicTexture> make_dynamic_texture(int startTextureIndex, int endTextureIndex);
};