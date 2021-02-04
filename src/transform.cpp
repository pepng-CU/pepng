#include "transform.hpp"

Transform::Transform() : Transform(glm::vec3(0.0f), glm::quat(), glm::vec3(1.0f)) {}

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : 
    Transform(
        position, 
        glm::quat(glm::radians(rotation)), 
        scale
    ) {}

Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale) : position(position), rotation(rotation), scale(scale) {}

glm::highp_mat4 Transform::getScaleMatrix(glm::highp_mat4 &matrix) {
    return glm::scale(matrix, this->scale);
}

glm::highp_mat4 Transform::getTranslateMatrix(glm::highp_mat4 &matrix) {
    return glm::translate(matrix, this->position);
}

glm::highp_mat4 Transform::getRotationMatrix() {
    return glm::toMat4(this->rotation);
}

glm::highp_mat4 Transform::getWorldMatrix() {
    auto matrix = glm::mat4(1.0f);

    matrix = this->getScaleMatrix(matrix);
    matrix = this->getRotationMatrix() * matrix;
    matrix = this->getTranslateMatrix(matrix);

    return matrix;
}

glm::highp_mat4 Transform::getViewMatrix() {
    auto matrix = glm::mat4(1.0f);

    matrix = this->getScaleMatrix(matrix);
    matrix = this->getRotationMatrix() * matrix;
    matrix = glm::translate(matrix, -this->position);

    return matrix;
}

glm::vec3 Transform::getRight() {
    auto rotationMatrix = this->getRotationMatrix();

    return glm::vec3(rotationMatrix[0][0], rotationMatrix[1][0], rotationMatrix[2][0]);
}

glm::vec3 Transform::getUp() {
    auto rotationMatrix = this->getRotationMatrix();

    return -glm::vec3(rotationMatrix[0][1], rotationMatrix[1][1], rotationMatrix[2][1]);
}

glm::vec3 Transform::getForward() {
    auto rotationMatrix = this->getRotationMatrix();

    return -glm::vec3(rotationMatrix[0][2], rotationMatrix[1][2], rotationMatrix[2][2]);
}

void Transform::deltaRotate(float deltaYaw, float deltaPitch) {
    this->rotation = glm::quat(
        glm::radians(
            glm::vec3(
                deltaPitch,
                0.0f,
                0.0f
            )
        )
    ) * this->rotation * glm::quat(
        glm::radians(
            glm::vec3(
                0.0f,
                deltaYaw,
                0.0f
            )
        )
    );
}

void Transform::relativeTranslate(glm::vec3 delta) {
    this->position += this->getForward() * delta.x
        + this->getUp() * delta.y
        + this->getRight() * delta.z;
}