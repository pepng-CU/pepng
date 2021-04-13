#pragma once

#include "light.hpp"

class Pointlight : public Light {
    public:
        static std::shared_ptr<Pointlight> make_point_light(GLuint shader_program, glm::vec3 color, float intensity);

        virtual void delayed_init() override;
        virtual void init_fbo() override;
        virtual void render(GLuint shader_program) override;

        glm::mat4 matrix();
        glm::mat4 projection();

        #ifdef IMGUI
        virtual void imgui() override;
        #endif

    protected:
        virtual Pointlight* clone_implementation() override;

        Pointlight(GLuint shader_program, glm::vec3 color, float intensity);
        Pointlight(const Pointlight& light);

    private:
        int __index;
        static int __count;
};

namespace pepng {
    std::shared_ptr<Pointlight> make_point_light(GLuint shader_program, glm::vec3 color, float intensity);
}