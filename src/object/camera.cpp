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

std::ostream& operator<<(std::ostream& os, const Camera& camera) {
    os  << "Camera { " 
        << (Transform&) camera 
        << " }";

    return os;
}

bool Viewport::render(glm::vec2 windowDimension) {
    glViewport(this->position.x, this->position.y, this->scale.x * windowDimension.x, this->scale.y * windowDimension.y);

    return true;
}

Projection::Projection(float aspect) : aspect(aspect) {}

void Projection::setAspect(float aspect) {
    this->aspect = aspect;
}

Perspective::Perspective(float fovy, float aspect, float near, float far) : Projection(aspect), fovy(fovy), near(near), far(far) {}

glm::mat4 Perspective::getMatrix() {
    return glm::perspective(this->fovy, this->aspect, this->near, this->far);
}