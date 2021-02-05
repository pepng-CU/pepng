#include "camera.hpp"

Camera::Camera(Transform transform, Viewport viewport, glm::mat4 projection) : Transform(transform), viewport(viewport), projection(projection) {}

bool Viewport::render() {
    glViewport(this->position.x, this->position.y, this->scale.x, this->scale.y);

    return true;
}

void Camera::render(GLuint shaderProgram) {
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
        glm::value_ptr(this->getViewMatrix())
    );
}

glm::vec3 Camera::getRight() {
    auto rotationMatrix = this->getRotationMatrix();

    return glm::vec3(rotationMatrix[0][0], rotationMatrix[1][0], rotationMatrix[2][0]);
}

glm::vec3 Camera::getUp() {
    auto rotationMatrix = this->getRotationMatrix();

    return -glm::vec3(rotationMatrix[0][1], rotationMatrix[1][1], rotationMatrix[2][1]);
}

glm::vec3 Camera::getForward() {
    auto rotationMatrix = this->getRotationMatrix();

    return glm::vec3(rotationMatrix[0][2], rotationMatrix[1][2], rotationMatrix[2][2]);
}

glm::quat Camera::getRotation() {
    return this->rotationX * this->rotationY * this->rotationZ;
}