#include "light.hpp"

Light::Light(GLuint shaderProgram, glm::vec3 color) : 
    Component("Light"), 
    shaderProgram(shaderProgram),
    near(0.1f),
    far(50.0f),
    dimension(10.0f),
    textureDimension(1024),
    color(color)
{}

Light::Light(const Light& light) : 
    Component(light), 
    shaderProgram(light.shaderProgram),
    near(light.near),
    far(light.far),
    dimension(light.dimension),
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

    
}

void Light::initFBO() {
    this->delayedInit();

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->texture, 0);

    glUseProgram(this->shaderProgram);

    glUniformMatrix4fv(
        glGetUniformLocation(this->shaderProgram, "u_light_space_matrix"),
        1,
        GL_FALSE,
        glm::value_ptr(this->getMatrix())
    );
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
    return glm::ortho(-this->dimension, this->dimension, -this->dimension, this->dimension, this->near, this->far);
}

glm::mat4 Light::getMatrix() {
    return this->getProjection() * this->transform->getViewMatrix();
}

void Light::render(GLuint shaderProgram) {
    if(!this->isActive) {
        return;
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->texture);

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "u_light_space_matrix"),
        1,
        GL_FALSE,
        glm::value_ptr(this->getMatrix())
    );

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
}

void Light::imgui() {
    ImGui::InputFloat("Near", &this->near);
    ImGui::InputFloat("Far", &this->far);
    ImGui::InputFloat("Box Dimension", &this->dimension);
    ImGui::ColorPicker3("Color", glm::value_ptr(this->color));
}

namespace pepng {
    std::shared_ptr<Light> makeLight(GLuint shaderProgram, glm::vec3 color) {
        return Light::makeLight(shaderProgram, color);
    }
}