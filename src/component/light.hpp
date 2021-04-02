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
        GLuint shader_program;

        static std::shared_ptr<Light> make_light(GLuint shaderProgram, glm::vec3 color);
        static std::vector<std::shared_ptr<Light>> lights;
        virtual void delayed_init() override;
        void init_fbo();
        void update_fbo();
        glm::mat4 matrix();
        glm::mat4 projection();
        void render(GLuint shaderProgram);
        virtual void init(std::shared_ptr<WithComponents> parent) override;

        #if IMGUI
        virtual void imgui() override;
        #endif

    protected:
        Light(GLuint shaderProgram, glm::vec3 color);
        Light(const Light& light);

        virtual Light* clone_implementation() override;

    private:
        GLuint fbo;
        GLuint texture;
        float near;
        float far;
        float texture_dimension;
        float intensity;
        glm::vec3 color;
        std::shared_ptr<Transform> transform;
};

namespace pepng {
    std::shared_ptr<Light> make_light(GLuint shaderProgram, glm::vec3 color);
}