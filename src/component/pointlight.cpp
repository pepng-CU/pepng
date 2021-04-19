#include "pointlight.hpp"

/**
 * STATICS
 */

int Pointlight::__count = 0;

Pointlight::Pointlight(GLuint shader_program, glm::vec3 color, float intensity) : 
    Light(shader_program, color, intensity)
{
    this->_name = "Pointlight";
}

Pointlight::Pointlight(const Pointlight& light) : 
    Light(light)
{}

Pointlight* Pointlight::clone_implementation() {
    return new Pointlight(*this);
}

std::shared_ptr<Pointlight> Pointlight::make_point_light(GLuint shader_program, glm::vec3 color, float intensity) {
    std::shared_ptr<Pointlight> light(new Pointlight(shader_program, color, intensity));

    Light::lights.push_back(light);

    return light;
}

std::shared_ptr<Pointlight> pepng::make_point_light(GLuint shader_program, glm::vec3 color, float intensity) {
    return Pointlight::make_point_light(shader_program, color, intensity);
}

void Pointlight::delayed_init() {
    if(this->_is_init) return;
    
    this->_is_init = true;

    glGenFramebuffers(1, &this->_fbo);

    glGenTextures(1, &this->_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->_texture);

    for(int i = 0; i < 6; i++) {
        #ifdef EMSCRIPTEN
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
            0, 
            GL_DEPTH_COMPONENT24, 
            1024,
            1024,
            0,
            GL_DEPTH_COMPONENT,
            GL_UNSIGNED_INT,
            NULL
        );
        #else
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
            0, 
            GL_DEPTH_COMPONENT32, 
            1024,
            1024,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            NULL
        );
        #endif
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    glBindFramebuffer(GL_FRAMEBUFFER, this->_fbo);

    #ifdef EMSCRIPTEN
    for(int i = 0; i < 6; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->_texture, 0);
    }
    #else
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->_texture, 0);
    #endif
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Pointlight::init_fbo() {
    this->delayed_init();

    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_fbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(this->_shader_program);

    auto light_position = this->_transform->position;

    glUniform3fv(
        glGetUniformLocation(this->_shader_program, "u_light_pos"),
        1,
        glm::value_ptr(light_position)
    );

    glUniform1f(
        glGetUniformLocation(this->_shader_program, "u_far"),
        this->_far
    );

    auto shadow_projection = this->projection();

    std::vector<glm::mat4> shadowTransforms;
    
    shadowTransforms.push_back(shadow_projection * 
                    glm::lookAt(light_position, light_position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
    shadowTransforms.push_back(shadow_projection * 
                    glm::lookAt(light_position, light_position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
    shadowTransforms.push_back(shadow_projection * 
                    glm::lookAt(light_position, light_position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadow_projection * 
                    glm::lookAt(light_position, light_position + glm::vec3(0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
    shadowTransforms.push_back(shadow_projection * 
                    glm::lookAt(light_position, light_position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
    shadowTransforms.push_back(shadow_projection * 
                    glm::lookAt(light_position, light_position + glm::vec3(0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));
    
    glUniformMatrix4fv(
        glGetUniformLocation(this->_shader_program, "u_shadow_matrices"),
        6,
        GL_FALSE,
        glm::value_ptr(shadowTransforms[0])
    );
}

glm::mat4 Pointlight::projection() {
    return glm::perspective(glm::radians(90.0f), 1.0f, this->_near, this->_far); 
}

glm::mat4 Pointlight::matrix() {
    return this->projection() * this->_transform->view_matrix();
}

void Pointlight::render(GLuint shader_program) {
    std::stringstream ss;

    ss << "u_pointlights[" << this->__index << "]";

    auto struct_prefix = ss.str();

    glUniform1i(
        glGetUniformLocation(shader_program, (struct_prefix + ".is_active").c_str()),
        this->_is_active
    );

    if(!this->_is_active) return;

    glActiveTexture(GL_TEXTURE2 + this->_texture_index);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->_texture);

    std::stringstream shadow_texture;

    shadow_texture << "u_point_shadows[" << this->__index << "]";

    glUniform1i(
        glGetUniformLocation(shader_program, shadow_texture.str().c_str()), 
        2 + this->_texture_index
    );

    auto light_position = this->_transform->position;
    auto light_direction = -this->_transform->forward();

    glUniform3fv(
        glGetUniformLocation(shader_program, (struct_prefix + ".position").c_str()),
        1,
        glm::value_ptr(light_position)
    );

    glUniform1f(
        glGetUniformLocation(shader_program, (struct_prefix + ".range").c_str()),
        this->_far
    );

    glUniform3fv(
        glGetUniformLocation(shader_program, (struct_prefix + ".color").c_str()),
        1,
        glm::value_ptr(this->_color)
    );

    glUniform1f(
        glGetUniformLocation(shader_program, (struct_prefix + ".intensity").c_str()),
        this->_intensity
    );

    glUniform1f(
        glGetUniformLocation(shader_program, (struct_prefix + ".shadows").c_str()),
        this->_shadows
    );
}

#ifdef IMGUI
void Pointlight::imgui() {
    Light::imgui();
}
#endif