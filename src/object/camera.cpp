#include "camera.hpp"

Camera::Camera(std::shared_ptr<CameraTransform> transform, Viewport viewport, std::shared_ptr<Projection> projection) :
    Object("Camera", transform),
    viewport(viewport),
    projection(projection)
{}

std::shared_ptr<Camera> Camera::currentCamera = nullptr;

void Camera::render(GLuint shaderProgram) {
    auto transform = this->getComponent<Transform>();

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "u_projection"),
        1,
        GL_FALSE,
        glm::value_ptr(this->projection->getMatrix())
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "u_view"),
        1,
        GL_FALSE,
        glm::value_ptr(transform->getViewMatrix())
    );
}

void Camera::imgui() {
    Object::imgui();

    if (ImGui::CollapsingHeader("Camera")) {
        this->viewport.imgui();
        this->projection->imgui();
    }
}

Viewport::Viewport(glm::vec2 position, glm::vec2 scale) : position(position), scale(scale), isActive(true) {}

bool Viewport::render(glm::vec2 windowDimension) {
    if(!this->isActive) {
        return false;
    }

    glViewport(
        this->position.x * windowDimension.x, 
        this->position.y * windowDimension.y, 
        this->scale.x * windowDimension.x, 
        this->scale.y * windowDimension.y
    );

    return true;
}

void Viewport::imgui() {
    ImGui::Checkbox("Active", &this->isActive);
    ImGui::InputFloat2("Position", glm::value_ptr(this->position));
    ImGui::InputFloat2("Scale", glm::value_ptr(this->scale));
}

Projection::Projection(float aspect) : aspect(aspect) {}

void Projection::setAspect(float aspect) {
    this->aspect = aspect;
}

Perspective::Perspective(float fovy, float aspect, float near, float far) : Projection(aspect), fovy(fovy), near(near), far(far) {}

glm::mat4 Perspective::getMatrix() {
    return glm::perspective(this->fovy, this->aspect, this->near, this->far);
}

void Perspective::imgui() {
    ImGui::InputFloat("Fovy", &this->fovy);
    ImGui::InputFloat("Near", &this->near);
    ImGui::InputFloat("Far", &this->far);
}