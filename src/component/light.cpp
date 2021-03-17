#include "light.hpp"

Light::Light(GLuint shaderProgram, glm::vec3 color) : 
    Component("Light"), 
    shaderProgram(shaderProgram),
    near(0.1f),
    far(50.0f),
    textureDimension(2048),
    color(color)
{}

Light::Light(const Light& light) : 
    Component(light), 
    shaderProgram(light.shaderProgram),
    near(light.near),
    far(light.far),
    textureDimension(light.textureDimension),
    color(light.color)
{}

std::vector<std::shared_ptr<Light>> Light::lights;

void Light::delayedInit() {
    if(this->isInit) {
        return;
    }
    
    this->isInit = true;

    glGenFramebuffers(1, &this->fbo);

    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);

    for(int i = 0; i < 6; i++) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
            0, 
            GL_DEPTH_COMPONENT, 
            this->textureDimension,
            this->textureDimension,
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
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::initFBO() {
    this->delayedInit();

    glViewport(0, 0, this->textureDimension, this->textureDimension);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(this->shaderProgram);

    glUniform3fv(
        glGetUniformLocation(shaderProgram, "u_light_pos"),
        1,
        glm::value_ptr(this->transform->position)
    );

    glUniform1f(
        glGetUniformLocation(shaderProgram, "u_far"),
        this->far
    );

    auto shadowProj = this->getProjection();
    auto lightPos = this->transform->position;

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
            glGetUniformLocation(shaderProgram, "u_shadow_matrices"),
            6,
            GL_FALSE,
            glm::value_ptr(shadowTransforms[0])
        );
    }
}

void Light::updateFBO() {
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::init(std::shared_ptr<WithComponents> parent) {
    this->transform = parent->getComponent<Transform>();

    if(this->transform == nullptr) {
        throw std::runtime_error("Light does not have a transform.");
    }
}

std::shared_ptr<Light> Light::makeLight(GLuint shaderProgram, glm::vec3 color) {
    std::shared_ptr<Light> light(new Light(shaderProgram, color));
    lights.push_back(light);

    return light;
}

Light* Light::cloneImplementation() {
    return new Light(*this);
}

glm::mat4 Light::getProjection() {
    float aspect = 1.0;
    return glm::perspective(glm::radians(90.0f), aspect, this->near, this->far); 
}

glm::mat4 Light::getMatrix() {
    return this->getProjection() * this->transform->getViewMatrix();
}

void Light::render(GLuint shaderProgram) {
    if(!this->isActive) {
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
}

void Light::imgui() {
    ImGui::InputFloat("Near", &this->near);
    ImGui::InputFloat("Far", &this->far);
    ImGui::ColorPicker3("Color", glm::value_ptr(this->color));
}

namespace pepng {
    std::shared_ptr<Light> makeLight(GLuint shaderProgram, glm::vec3 color) {
        return Light::makeLight(shaderProgram, color);
    }
}