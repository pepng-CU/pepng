#pragma once

#include "light.hpp"

class PointLight : public Light {
    public:
        static std::shared_ptr<PointLight> make_point_light(GLuint shader_program, glm::vec3 color, float intensity);

        virtual void delayed_init() override;
        virtual void init_fbo() override;
        virtual void update_fbo() override;
        virtual void render(GLuint shader_program) override;

        glm::mat4 matrix();
        glm::mat4 projection();

        #ifdef IMGUI
        virtual void imgui() override;
        #endif

    protected:
        virtual PointLight* clone_implementation() override;

        PointLight(GLuint shader_program, glm::vec3 color, float intensity);
        PointLight(const PointLight& light);

    private:
        int __index;
        static int __count;
};

namespace pepng {
    std::shared_ptr<PointLight> make_point_light(GLuint shader_program, glm::vec3 color, float intensity);
}