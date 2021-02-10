#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "object.hpp"
#include "../component/components.hpp"

class Viewport : public WithImGui {
    public:
        bool isActive;
        glm::vec2 position;
        glm::vec2 scale;

        Viewport(glm::vec2 position, glm::vec2 scale);

        /**
         * Attempts to render the viewport.
         * Returns if the viewport is visible or not.
         */
        bool render(glm::vec2 windowDimension);

        virtual void imgui() override;
};

class Projection : public WithImGui {
    public:
        Projection(float aspect);

        virtual glm::mat4 getMatrix() = 0;

        void setAspect(float aspect);
    protected:
        float aspect;
};

class Perspective : public Projection {
    public:
        Perspective(float fovy, float aspect, float near, float far);

        virtual glm::mat4 getMatrix() override;

        virtual void imgui() override;

    private:
        float fovy;
        float near;
        float far;
};

class Camera : public Object {
    public:
        static std::shared_ptr<Camera> currentCamera;

        std::shared_ptr<Projection> projection;
        
        Viewport viewport;

        Camera(std::shared_ptr<CameraTransform> transform, Viewport viewport, std::shared_ptr<Projection> projection);

        void render(GLuint shaderProgram);

        virtual void imgui() override;
};