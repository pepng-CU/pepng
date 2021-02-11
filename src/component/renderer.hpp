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

        static std::shared_ptr<Renderer> makeRenderer(std::shared_ptr<Model> model, GLuint shaderProgram, GLuint texture, GLenum renderMode);

        virtual void update(std::shared_ptr<WithComponents> object) override;
        virtual void imgui() override;
    private:
        Renderer(std::shared_ptr<Model> model, GLuint shaderProgram, GLuint texture, GLenum renderMode);
};

namespace pepng {
    std::shared_ptr<Renderer> makeRenderer(std::shared_ptr<Model> model, GLuint shaderProgram = -1, GLuint texture = 1, GLenum renderMode = GL_TRIANGLES);
};