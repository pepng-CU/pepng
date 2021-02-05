#include "transform.hpp"

Transform::Transform() : Transform(glm::vec3(0.0f), glm::quat(glm::vec3(0.0f)), glm::vec3(1.0f)) {}

Transform::Transform(const Transform &transform) : Transform(glm::vec3(transform.position), glm::quat(transform.rotation), glm::vec3(transform.scale)) {}

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

    matrix = this->getTranslateMatrix(matrix);
    matrix *= this->getRotationMatrix();
    matrix = this->getScaleMatrix(matrix);

    return matrix;
}

glm::highp_mat4 Transform::getViewMatrix() {
    auto matrix = glm::mat4(1.0f);

    matrix = glm::translate(matrix, -this->position);
    matrix = this->getRotationMatrix() * matrix;

    return matrix;
}

glm::vec3 Transform::getRight() {
    return this->rotation * glm::vec3(-1.0f, 0.0f, 0.0f);
}

glm::vec3 Transform::getUp() {
    return this->rotation * glm::vec3(0.0f, -1.0f, 0.0f);
}

glm::vec3 Transform::getForward() {
    return this->rotation * glm::vec3(0.0f, 0.0f, 1.0f);
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