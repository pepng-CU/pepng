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
        static std::vector<std::shared_ptr<Light>> lights;

        virtual void init_fbo() = 0;
        virtual void update_fbo();
        virtual void render(GLuint shaderProgram) = 0;

        inline GLuint shader_program() { return _shader_program; }

        virtual void init(std::shared_ptr<WithComponents> parent) override;

        #ifdef IMGUI
        virtual void imgui() override;
        #endif

    protected:
        Light(GLuint shader_program, glm::vec3 color, float intensity);
        Light(const Light& light);

        std::shared_ptr<Transform> _transform;
        GLuint _texture;
        int _texture_index;
        GLuint _fbo;
        GLuint _shader_program;
        bool _shadows;
        float _near;
        float _far;
        float _intensity;
        glm::vec3 _color;
    
    private:
        int __count;
};