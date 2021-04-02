#pragma once

#include <vector>
#include <memory>

#include <GL/glew.h>

#include "component.hpp"
#include "with_components.hpp"
#include "transform.hpp"
#include "../util/delayed_init.hpp"

class Light : public Component, public DelayedInit {
    public:
        GLuint shaderProgram;

        static std::shared_ptr<Light> makeLight(GLuint shaderProgram, glm::vec3 color);
        static std::vector<std::shared_ptr<Light>> lights;
        virtual void delayedInit() override;
        void initFBO();
        void updateFBO();
        glm::mat4 getMatrix();
        void render(GLuint shaderProgram);
        virtual void imgui() override;
        virtual void init(std::shared_ptr<WithComponents> parent) override;

    protected:
        Light(GLuint shaderProgram, glm::vec3 color);
        Light(const Light& light);

        virtual Light* cloneImplementation() override;

        glm::mat4 getProjection();

        GLuint fbo;
        GLuint texture;
        float near;
        float far;
        float textureDimension;
        float intensity;
        glm::vec3 color;
        std::shared_ptr<Transform> transform;
};

namespace pepng {
    std::shared_ptr<Light> makeLight(GLuint shaderProgram, glm::vec3 color);
}