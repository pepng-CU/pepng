#include "transform.hpp"

Transform::Transform() :
    Transform(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    )
{}

Transform::Transform(const Transform &transform) : 
    position(transform.position), 
    rotationX(glm::quat(transform.rotationX)), 
    rotationY(glm::quat(transform.rotationY)), 
    rotationZ(glm::quat(transform.rotationZ)), 
    scale(transform.scale),
    parentMatrix(transform.parentMatrix)
{}

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : 
    position(position), 
    rotationX(glm::quat(glm::vec3(0.0f))), 
    rotationY(glm::quat(glm::vec3(0.0f))), 
    rotationZ(glm::quat(glm::vec3(0.0f))), 
    scale(scale),
    parentMatrix(glm::mat4(1.0f))
{
    this->setRotation(rotation);
}

Transform::Transform(
    glm::vec3 position, 
    glm::quat rotationX, 
    glm::quat rotationY,
    glm::quat rotationZ, 
    glm::vec3 scale
) : position(position), 
    rotationX(rotationX), 
    rotationY(rotationY),
    rotationZ(rotationZ),
    scale(scale) 
{}

glm::quat Transform::getRotation() {
    return this->rotationZ * this->rotationY * this->rotationX;
}

glm::highp_mat4 Transform::getScaleMatrix(glm::highp_mat4 &matrix) {
    return glm::scale(matrix, this->scale);
}

glm::highp_mat4 Transform::getTranslateMatrix(glm::highp_mat4 &matrix) {
    return glm::translate(matrix, this->position);
}

glm::highp_mat4 Transform::getRotationMatrix() {
    return glm::toMat4(this->getRotation());
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
    return this->getRotation() * glm::vec3(-1.0f, 0.0f, 0.0f);
}

glm::vec3 Transform::getUp() {
    return this->getRotation() * glm::vec3(0.0f, -1.0f, 0.0f);
}

glm::vec3 Transform::getForward() {
    return this->getRotation() * glm::vec3(0.0f, 0.0f, 1.0f);
}

void Transform::deltaRotate(glm::vec3 degDelta) {
    glm::vec3 delta = glm::radians(degDelta);

    this->rotationX *= glm::quat(glm::vec3(delta.x, 0.0f, 0.0f));
    this->rotationY *= glm::quat(glm::vec3(0.0f, delta.y, 0.0f));
    this->rotationZ *= glm::quat(glm::vec3(0.0f, 0.0f, delta.z));
}

void Transform::setRotation(glm::vec3 degRotation) {
    glm::vec3 rotation = glm::radians(degRotation);

    this->rotationX = glm::quat(glm::vec3(rotation.x, 0.0f, 0.0f));
    this->rotationY = glm::quat(glm::vec3(0.0f, rotation.y, 0.0f));
    this->rotationZ = glm::quat(glm::vec3(0.0f, 0.0f, rotation.z));
}

void Transform::setRotation(const Transform& transform) {
    this->rotationX = glm::quat(transform.rotationX);
    this->rotationY = glm::quat(transform.rotationY);
    this->rotationZ = glm::quat(transform.rotationZ);
}

void Transform::relativeTranslate(glm::vec3 delta) {
    this->position 
        += this->getForward() * delta.x
        + this->getUp() * delta.y
        + this->getRight() * delta.z;
}

glm::vec3 Transform::getEuler() {
    return glm::eulerAngles(this->getRotation());
}

void Transform::imgui() {
    if(ImGui::CollapsingHeader("Transform")) {
        glm::vec3 rotation = glm::degrees(this->getEuler());

        ImGui::InputFloat3("Position", glm::value_ptr(this->position));
        ImGui::InputFloat3("Rotation", glm::value_ptr(rotation));
        ImGui::InputFloat3("Scale", glm::value_ptr(this->scale));
    }
}


std::ostream& operator<<(std::ostream& os, const Transform& transform) {
    os  << "Transform { Position: " 
        << glm::to_string(transform.position) 
        << ", Rotation: " 
        << glm::to_string(transform.rotationX * transform.rotationY * transform.rotationZ) 
        << ", Scale: " 
        << glm::to_string(transform.scale)
        << " }";

    return os;
}