#pragma once

#include <GL/glew.h>
#include <memory>
#include <glm/gtx/string_cast.hpp>

#include "component.hpp"
#include "../component/light.hpp"
#include "../gl/model.hpp"
#include "../gl/material.hpp"

/**
 * The Rendering component for objects.
 */
class Renderer : public Component {
    public:
        /**
         * The OpenGL render mode.
         */
        GLenum renderMode;
        /**
         * Pointer to the model.
         */
        std::shared_ptr<Model> model;
        /**
         * Pointer to the material.
         */
        std::shared_ptr<Material> material;

        /**
         * Shared_ptr constructor for Renderer.
         */
        static std::shared_ptr<Renderer> makeRenderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum renderMode);

        void render(std::shared_ptr<WithComponents> object, GLuint shaderProgram);
        virtual void render(std::shared_ptr<WithComponents> object) override;
        virtual void imgui() override;

        virtual std::shared_ptr<Component> clone() override;
        virtual std::shared_ptr<Renderer> clone1();
    private:
        Renderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum renderMode);
        Renderer(const Renderer& renderer);
};

namespace pepng {
    std::shared_ptr<Renderer> makeRenderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum renderMode = GL_TRIANGLES);
};