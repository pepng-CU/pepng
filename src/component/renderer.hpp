#pragma once

#include <GL/glew.h>
#include <memory>
#include <glm/gtx/string_cast.hpp>

#include "component.hpp"
#include "../gl/model.hpp"

class Renderer : public Component {
    public:
        GLenum renderMode;
        GLuint shaderProgram;
        GLuint texture;
        std::shared_ptr<Model> model;

        Renderer(std::shared_ptr<Model> model);
        Renderer(std::shared_ptr<Model> model, GLuint shaderProgram, GLuint texture, GLenum renderMode = GL_TRIANGLES);

        virtual void update(std::shared_ptr<WithComponents> object) override;
        virtual void imgui() override;
};