#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "object.hpp"
#include "../component/components.hpp"

class Viewport {
    public:
        glm::vec2 position;
        glm::vec2 scale;

        /**
         * Attempts to render the viewport.
         * Returns if the viewport is visible or not.
         */
        bool render(glm::vec2 windowDimension);
};

class Projection {
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

        friend std::ostream& operator<<(std::ostream& os, const Camera& camera);
};

std::ostream& operator<<(std::ostream& os, const Camera& camera);