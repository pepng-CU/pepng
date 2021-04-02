#pragma once

#include <memory>

#include <GL/glew.h>
#include <glm/gtx/string_cast.hpp>

#include "component.hpp"
#include "renderer.hpp"
#include "../gl/model.hpp"
#include "../gl/material.hpp"

/**
 * Component that switches the Renderer's texture every ~10 seconds.
 */
class DynamicTexture : public Component {
    public:
        /**
         * The first texture index.
         */
        int startTextureIndex;

        /**
         * The last texture index.
         */
        int endTextureIndex;

        /**
         * Current active texture.
         */
        int currentIndex;

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
        static std::shared_ptr<DynamicTexture> makeDynamicTexture(int startTextureIndex, int endTextureIndex);

        virtual void update(std::shared_ptr<WithComponents> object) override;
        virtual void imgui() override;

        virtual DynamicTexture* cloneImplementation() override;
    private:
        DynamicTexture(int startTextureIndex, int endTextureIndex);
        DynamicTexture(const DynamicTexture& renderer);
};

namespace pepng {
    std::shared_ptr<DynamicTexture> makeDynamicTexture(int startTextureIndex, int endTextureIndex);
};