#include "camera.hpp"

Camera::Camera(Transform transform, Viewport viewport, glm::mat4 projection) : transform(transform), viewport(viewport), projection(projection) {}

void Viewport::render() {
    glViewport(this->position.x, this->position.y, this->scale.x, this->scale.y);
}

void Camera::render(GLuint shaderProgram) {
    this->viewport.render();

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "u_projection"),
        1,
        GL_FALSE,
        glm::value_ptr(this->projection)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "u_view"),
        1,
        GL_FALSE,
        glm::value_ptr(this->transform.getViewMatrix())
    );
}