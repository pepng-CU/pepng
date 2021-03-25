#include "transform.hpp"

Transform::Transform(const Transform &transform) : 
    Transform(
        transform.position,
        transform.rotationX,
        transform.rotationY,
        transform.rotationZ,
        transform.scale,
        transform.shear
    )
{
    this->parentMatrix = glm::mat4(transform.parentMatrix);
}

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 shear) : 
    Transform(
        position,
        glm::quat(glm::vec3(0.0f)),
        glm::quat(glm::vec3(0.0f)),
        glm::quat(glm::vec3(0.0f)),
        scale,
        shear
    )
{
    this->setRotation(rotation);
}

Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 shear) : 
    Transform(
        position,
        glm::eulerAngles(rotation),
        scale,
        shear
    )
{}

Transform::Transform(
    glm::vec3 position, 
    glm::quat rotationX, 
    glm::quat rotationY,
    glm::quat rotationZ, 
    glm::vec3 scale,
    glm::vec3 shear
) : 
    Component("Transform"),
    position(position), 
    rotationX(rotationX), 
    rotationY(rotationY),
    rotationZ(rotationZ),
    scale(scale),
    shear(shear),
    parentMatrix(glm::mat4(1.0f))
{}

std::shared_ptr<Transform> Transform::makeTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 shear) {
    std::shared_ptr<Transform> transform(new Transform(position, rotation, scale, shear));

    return transform;
}

std::shared_ptr<Transform> Transform::makeTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 shear) {
    std::shared_ptr<Transform> transform(new Transform(position, rotation, scale, shear));

    return transform;
}

Transform* Transform::cloneImplementation() {
    return new Transform(*this);
}

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

glm::highp_mat4 Transform::getShearMatrix(glm::highp_mat4 &matrix) {
    // TODO

    return glm::shearX3D(matrix, this->shear.x, this->shear.z);
}

glm::highp_mat4 Transform::getWorldMatrix() {
    auto matrix = glm::mat4(1.0f);

    matrix = this->getTranslateMatrix(matrix);
    matrix *= this->getRotationMatrix();
    matrix = this->getScaleMatrix(matrix);
    matrix = this->getShearMatrix(matrix);

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
    glm::vec3 rotation = glm::degrees(this->getEuler());

    ImGui::InputFloat3("Position", glm::value_ptr(this->position));
    ImGui::InputFloat3("Rotation", glm::value_ptr(rotation));
    ImGui::InputFloat3("Scale", glm::value_ptr(this->scale));
    ImGui::InputFloat3("Shear", glm::value_ptr(this->shear));
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

CameraTransform::CameraTransform(const CameraTransform &transform) : Transform(transform) {}

CameraTransform::CameraTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 shear) : Transform(position, rotation, scale, shear) {}

CameraTransform::CameraTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 shear) : Transform(position, rotation, scale, shear) {}

std::shared_ptr<CameraTransform> CameraTransform::makeCameraTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 shear) {
    std::shared_ptr<CameraTransform> transform(new CameraTransform(position, rotation, scale, shear));

    return transform;
}

std::shared_ptr<CameraTransform> CameraTransform::makeCameraTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 shear) {
    std::shared_ptr<CameraTransform> transform(new CameraTransform(position, rotation, scale, shear));

    return transform;
}

CameraTransform* CameraTransform::cloneImplementation() {
    return new CameraTransform(*this);
}

glm::vec3 CameraTransform::getRight() {
    auto rotationMatrix = this->getRotationMatrix();

    return glm::vec3(rotationMatrix[0][0], rotationMatrix[1][0], rotationMatrix[2][0]);
}

glm::vec3 CameraTransform::getUp() {
    auto rotationMatrix = this->getRotationMatrix();

    return -glm::vec3(rotationMatrix[0][1], rotationMatrix[1][1], rotationMatrix[2][1]);
}

glm::vec3 CameraTransform::getForward() {
    auto rotationMatrix = this->getRotationMatrix();

    return glm::vec3(rotationMatrix[0][2], rotationMatrix[1][2], rotationMatrix[2][2]);
}

glm::quat CameraTransform::getRotation() {
    return this->rotationX * this->rotationY * this->rotationZ;
}

namespace pepng {
    std::shared_ptr<Transform> makeTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 shear) {
        return Transform::makeTransform(position, rotation, scale, shear);
    }

    std::shared_ptr<Transform> makeTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 shear) {
        return Transform::makeTransform(position, rotation, scale, shear);
    }

    std::shared_ptr<CameraTransform> makeCameraTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 shear) {
        return CameraTransform::makeCameraTransform(position, rotation, scale, shear);
    }

    std::shared_ptr<CameraTransform> makeCameraTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 shear) {
        return CameraTransform::makeCameraTransform(position, rotation, scale, shear);
    }
}