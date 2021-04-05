#include "light.hpp"

Light::Light(GLuint shaderProgram, glm::vec3 color) : 
    Component("Light"), 
    shader_program(shaderProgram),
    near(0.1f),
    far(100.0f),
    texture_dimension(2048),
    color(color),
    intensity(1000)
{}

Light::Light(const Light& light) : 
    Component(light), 
    shader_program(light.shader_program),
    near(light.near),
    far(light.far),
    texture_dimension(light.texture_dimension),
    color(light.color),
    intensity(light.intensity)
{}

std::vector<std::shared_ptr<Light>> Light::lights;

void Light::delayed_init() {
    if(this->_is_init) {
        return;
    }
    
    this->_is_init = true;

    glGenFramebuffers(1, &this->fbo);

    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);

    for(int i = 0; i < 6; i++) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
            0, 
            GL_DEPTH_COMPONENT, 
            this->texture_dimension,
            this->texture_dimension,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            NULL
        );
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    #ifndef EMSCRIPTEN
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->texture, 0);
    #endif
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::init_fbo() {
    this->delayed_init();

    glViewport(0, 0, this->texture_dimension, this->texture_dimension);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(this->shader_program);

    auto lightPos = this->transform->position;

    glUniform3fv(
        glGetUniformLocation(shader_program, "u_light_pos"),
        1,
        glm::value_ptr(lightPos)
    );

    glUniform1f(
        glGetUniformLocation(shader_program, "u_far"),
        this->far
    );

    auto shadowProj = this->projection();

    std::vector<glm::mat4> shadowTransforms;
    
    shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos, lightPos + glm::vec3(0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
    shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));
    
    for(int i = 0; i < 6; i++) {
        glUniformMatrix4fv(
            glGetUniformLocation(shader_program, "u_shadow_matrices"),
            6,
            GL_FALSE,
            glm::value_ptr(shadowTransforms[0])
        );
    }
}

void Light::update_fbo() {
    #ifndef EMSCRIPTEN
    glDrawBuffer(GL_NONE);
    #endif
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::init(std::shared_ptr<WithComponents> parent) {
    this->transform = parent->get_component<Transform>();

    if(this->transform == nullptr) {
        std::cout << "Light does not have a transform." << std::endl;

        throw std::runtime_error("Light does not have a transform.");
    }
}

std::shared_ptr<Light> Light::make_light(GLuint shaderProgram, glm::vec3 color) {
    std::shared_ptr<Light> light(new Light(shaderProgram, color));
    lights.push_back(light);

    return light;
}

Light* Light::clone_implementation() {
    return new Light(*this);
}

glm::mat4 Light::projection() {
    float aspect = 1.0;
    return glm::perspective(glm::radians(90.0f), aspect, this->near, this->far); 
}

glm::mat4 Light::matrix() {
    return this->projection() * this->transform->view_matrix();
}

void Light::render(GLuint shaderProgram) {
    if(!this->_is_active) {
        return;
    }

    //glUseProgram(this->shaderProgram);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);

    glUniform3fv(
        glGetUniformLocation(shaderProgram, "u_light_pos"),
        1,
        glm::value_ptr(this->transform->position)
    );

    glUniform3fv(
        glGetUniformLocation(shaderProgram, "u_light_color"),
        1,
        glm::value_ptr(this->color)
    );

    glUniform1f(
        glGetUniformLocation(shaderProgram, "u_far"),
        this->far
    );

    glUniform1f(
        glGetUniformLocation(shaderProgram, "u_intensity"),
        this->intensity
    );
}

namespace pepng {
    std::shared_ptr<Light> make_light(GLuint shaderProgram, glm::vec3 color) {
        return Light::make_light(shaderProgram, color);
    }
}

#if IMGUI
void Light::imgui() {
    Component::imgui();

    ImGui::InputFloat("Near", &this->near);
    ImGui::InputFloat("Far", &this->far);
    ImGui::InputFloat("Intensity", &this->intensity);
    ImGui::ColorPicker3("Color", glm::value_ptr(this->color));
}
#endif