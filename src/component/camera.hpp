#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "components.hpp"

/**
 * Viewport used for glViewport (which uses relative position instead of absolute).
 */
class Viewport : public WithImGui {
    public:
        /**
         * Variable to check if the viewport will render.
         */
        bool isActive;
        /**
         * The XY position of the viewport.
         */
        glm::vec2 position;
        /**
         * The XY scale of the viewport.
         */
        glm::vec2 scale;

        /**
         * Shared_ptr constructor of Viewport.
         * 
         * @param position The XY viewport position.
         * @param scale The XY scale of the viewport.
         */
        static std::shared_ptr<Viewport> makeViewport(glm::vec2 position, glm::vec2 scale);

        /**
         * Attempts to render the viewport.
         * Returns if the viewport is visible or not.
         * 
         * @param windowDimension The XY dimension of the screen.
         */
        bool render(glm::vec2 windowDimension);

        virtual void imgui() override;

        virtual std::shared_ptr<Viewport> clone();
    private:
        Viewport(glm::vec2 position, glm::vec2 scale);
        Viewport(const Viewport& viewport);
};

/**
 * Abstract projection definition.
 */
class Projection : public WithImGui {
    public:
        virtual std::shared_ptr<Projection> clone() = 0;

        /**
         * Generates the projection matrix.
         */
        virtual glm::mat4 getMatrix() = 0;

        /**
         * Sets the projection aspect ratio.
         * 
         * @param aspect The aspect ratio to set.
         */
        void setAspect(float aspect);
        
    protected:
        Projection(float aspect);
        Projection(const Projection& projection);

        /**
         * The current window aspect ratio.
         */
        float aspect;
};

/**
 * The perspective projection.
 */
class Perspective : public Projection {
    public:
        /**
         * Shared_ptr constructor of Perspective.
         */
        static std::shared_ptr<Perspective> makePerspective(float fovy, float aspect, float near, float far);

        virtual std::shared_ptr<Projection> clone() override;

        virtual std::shared_ptr<Perspective> clone1();

        virtual glm::mat4 getMatrix() override;

        virtual void imgui() override;

    private:
        Perspective(float fovy, float aspect, float near, float far);
        Perspective(const Perspective& perspective);

        /**
         * The field of view of the camera.
         */
        float fovy;
        /**
         * The near cutoff plane.
         */
        float near;
        /**
         * The far cutoff plane.
         */
        float far;
};

/**
 * The Camera component used in rendering.
 */
class Camera : public Component {
    public:
        /**
         * Pointer to the current camera being rendered.
         */
        static std::shared_ptr<Camera> currentCamera;

        /**
         * Static list of cameras.
         */
        static std::vector<std::shared_ptr<Camera>> cameras;

        /**
         * Static_ptr constructor of Camera.
         */
        static std::shared_ptr<Camera> makeCamera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection);

        virtual std::shared_ptr<Component> clone() override;

        virtual std::shared_ptr<Camera> clone1();

        /**
         * Pointer to the projection used for the camera.
         */
        std::shared_ptr<Projection> projection;
        
        /**
         * Pointer to the camera viewport.
         */
        std::shared_ptr<Viewport> viewport;

        void render(GLuint shaderProgram);

        virtual void init(std::shared_ptr<WithComponents> parent) override;

        virtual void imgui() override;

    private:
        /**
         * Pointer to the parent component of the camera.
         * We store this because camera rendering doesn't fall in the update/rendering stage.
         */
        std::shared_ptr<WithComponents> parent;

        Camera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection);
        Camera(const Camera& camera);
};

namespace pepng {
    std::shared_ptr<Camera> makeCamera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection);

    std::shared_ptr<Viewport> makeViewport(glm::vec2 position, glm::vec2 scale);

    std::shared_ptr<Perspective> makePerspective(float fovy, float aspect, float near, float far);
};