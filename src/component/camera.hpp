#pragma once

#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "component.hpp"
#include "transform.hpp"

/**
 * Viewport used for glViewport (which uses relative position instead of absolute).
 */
class Viewport
    #ifdef IMGUI
    : public WithImGui 
    #endif
{
    public:
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
        static std::shared_ptr<Viewport> make_viewport(glm::vec2 position, glm::vec2 scale);

        /**
         * Accessor for active variable.
         */
        inline bool active() { return this->__is_active; }

        /**
         * Attempts to render the viewport.
         * Returns if the viewport is visible or not.
         * 
         * @param windowDimension The XY dimension of the screen.
         */
        bool render(glm::vec2 windowDimension);

        #ifdef IMGUI
        virtual void imgui() override;
        #endif

        virtual std::shared_ptr<Viewport> clone();
    private:
        Viewport(glm::vec2 position, glm::vec2 scale);
        Viewport(const Viewport& viewport);

        /**
         * Variable to check if the viewport will render.
         */
        bool __is_active;
};

/**
 * Abstract projection definition.
 */
class Projection 
    #ifdef IMGUI
    : public WithImGui 
    #endif
{
    public:
        virtual std::shared_ptr<Projection> clone() = 0;

        /**
         * Generates the projection matrix.
         */
        virtual glm::mat4 matrix() = 0;

        /**
         * Sets the projection aspect ratio.
         * 
         * @param aspect The aspect ratio to set.
         */
        inline void set_aspect(float aspect) { this->_aspect = aspect; }
        
    protected:
        Projection(float aspect);
        Projection(const Projection& projection);

        /**
         * The current window aspect ratio.
         */
        float _aspect;
};

/**
 * The perspective projection.
 */
class Perspective : public Projection {
    public:
        /**
         * Shared_ptr constructor of Perspective.
         */
        static std::shared_ptr<Perspective> make_perspective(float fovy, float aspect, float near, float far);

        virtual std::shared_ptr<Projection> clone() override;

        virtual std::shared_ptr<Perspective> clone1();

        virtual glm::mat4 matrix() override;

        #ifdef IMGUI
        virtual void imgui() override;
        #endif

    private:
        Perspective(float fovy, float aspect, float near, float far);
        Perspective(const Perspective& perspective);

        /**
         * The field of view of the camera.
         */
        float __fovy;
        /**
         * The near cutoff plane.
         */
        float __near;
        /**
         * The far cutoff plane.
         */
        float __far;
};

/**
 * The Camera component used in rendering.
 */
class Camera : public Component {
    public:
        /**
         * Pointer to the current camera being rendered.
         */
        static std::shared_ptr<Camera> current_camera;

        /**
         * Static list of cameras.
         */
        static std::vector<std::shared_ptr<Camera>> cameras;

        /**
         * Static_ptr constructor of Camera.
         */
        static std::shared_ptr<Camera> make_camera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection);

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

        #ifdef IMGUI
        virtual void imgui() override;
        #endif
        
    protected:
        virtual Camera* clone_implementation() override;

    private:
        /**
         * Pointer to the parent component of the camera.
         * We store this because camera rendering doesn't fall in the update/rendering stage.
         */
        std::shared_ptr<WithComponents> __parent;

        Camera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection);
        Camera(const Camera& camera);
};

namespace pepng {
    std::shared_ptr<Camera> make_camera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection);

    std::shared_ptr<Viewport> make_viewport(glm::vec2 position, glm::vec2 scale);

    std::shared_ptr<Perspective> make_perspective(float fovy, float aspect, float near, float far);
};