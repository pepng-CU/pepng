#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "transform.hpp"
#include "component.hpp"

class Viewport {
    public:
        glm::vec2 position;
        glm::vec2 scale;

        /**
         * Attempts to render the viewport.
         * Returns if the viewport is visible or not.
         */
        bool render();
};

class Camera : public Transform, public ComponentManager, public std::enable_shared_from_this<Camera> {
    public:
        Viewport viewport;

        Camera(Transform transform, Viewport viewport, glm::mat4 projection);

        void render(GLuint shaderProgram);

        virtual glm::vec3 getForward() override;

        virtual glm::vec3 getUp() override;

        virtual glm::vec3 getRight() override;

        virtual glm::quat getRotation() override;

        virtual void update() override {
            for(auto component : this->components) {
                component->update(shared_from_this());
            }
        }

        friend std::ostream& operator<<(std::ostream& os, const Camera& camera);

    private:
        glm::mat4 projection;
};

std::ostream& operator<<(std::ostream& os, const Camera& camera);