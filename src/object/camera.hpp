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

        static std::shared_ptr<Viewport> makeViewport(glm::vec2 position, glm::vec2 scale);

        /**
         * Attempts to render the viewport.
         * Returns if the viewport is visible or not.
         */
        bool render(glm::vec2 windowDimension);

        virtual void imgui() override;
    private:
        Viewport(glm::vec2 position, glm::vec2 scale);
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
        static std::shared_ptr<Perspective> makePerspective(float fovy, float aspect, float near, float far);

        virtual glm::mat4 getMatrix() override;

        virtual void imgui() override;

    private:
        Perspective(float fovy, float aspect, float near, float far);

        float fovy;
        float near;
        float far;
};

class Camera : public Object {
    public:
        static std::shared_ptr<Camera> currentCamera;

        static std::shared_ptr<Camera> makeCamera(std::shared_ptr<CameraTransform> transform, std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection);

        std::shared_ptr<Projection> projection;
        
        std::shared_ptr<Viewport> viewport;

        void render(GLuint shaderProgram);

        virtual void imgui() override;

    private:
        Camera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection);
};

namespace pepng {
    std::shared_ptr<Camera> makeCamera(std::shared_ptr<CameraTransform> transform, std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection);

    std::shared_ptr<Viewport> makeViewport(glm::vec2 position, glm::vec2 scale);

    std::shared_ptr<Perspective> makePerspective(float fovy, float aspect, float near, float far);
};