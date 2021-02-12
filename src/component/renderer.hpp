#pragma once

#include <GL/glew.h>
#include <memory>
#include <glm/gtx/string_cast.hpp>

#include "component.hpp"
#include "../gl/model.hpp"
#include "../gl/material.hpp"

class Renderer : public Component {
    public:
        GLenum renderMode;
        std::shared_ptr<Model> model;
        std::shared_ptr<Material> material;

        static std::shared_ptr<Renderer> makeRenderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum renderMode);

        virtual void update(std::shared_ptr<WithComponents> object) override;
        virtual void imgui() override;
    private:
        Renderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum renderMode);
};

namespace pepng {
    std::shared_ptr<Renderer> makeRenderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum renderMode = GL_TRIANGLES);
};