#pragma once

#include <memory>

#include <GL/glew.h>
#include <glm/gtx/string_cast.hpp>

#include "component.hpp"
#include "light.hpp"
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
        GLenum render_mode;
        /**
         * Pointer to the model.
         */
        std::shared_ptr<Model> model;
        /**
         * Pointer to the material.
         */
        std::shared_ptr<Material> material;
        /**
         * Does model receive shadow?
         */
        bool receive_shadow;
        /**
         * Does model display texture?
         */
        bool display_texture;

        /**
         * Shared_ptr constructor for Renderer.
         */
        static std::shared_ptr<Renderer> make_renderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum render_mode);

        void render(std::shared_ptr<WithComponents> object, GLuint shaderProgram);
        virtual void render(std::shared_ptr<WithComponents> object) override;

        virtual Renderer* clone_implementation() override;

        #if IMGUI
        virtual void imgui() override;
        #endif
    private:
        Renderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum render_mode);
        Renderer(const Renderer& renderer);
};

namespace pepng {
    std::shared_ptr<Renderer> make_renderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum render_mode = GL_TRIANGLES);
};