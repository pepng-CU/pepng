#include "transformer.hpp"
#include "../io/io.hpp"
#include "transform.hpp"

Transformer::Transformer(float positionSpeed, float rotationSpeed, float scaleSpeed) :
    Component("Transformer"),
    positionSpeed(positionSpeed),
    rotationSpeed(rotationSpeed),
    scaleSpeed(scaleSpeed)
{}

Transformer::Transformer(const Transformer& transformer) :
    Component(transformer),
    positionSpeed(transformer.positionSpeed),
    rotationSpeed(transformer.rotationSpeed),
    scaleSpeed(transformer.scaleSpeed)
{}

std::shared_ptr<Transformer> Transformer::makeTransformer(float positionSpeed, float rotationSpeed, float scaleSpeed) {
    std::shared_ptr<Transformer> transformer(new Transformer(positionSpeed, rotationSpeed, scaleSpeed));

    return transformer;
}

std::shared_ptr<Transformer> pepng::makeTransformer(float positionSpeed, float rotationSpeed, float scaleSpeed) {
    return Transformer::makeTransformer(positionSpeed, rotationSpeed, scaleSpeed);
}

Transformer* Transformer::cloneImplementation() {
    return new Transformer(*this);
}

void Transformer::update(std::shared_ptr<WithComponents> parent) {
    if(!this->isActive) {
        return;
    }

    auto transform = parent->getComponent<Transform>();

    auto input = Input::get();

    auto deltaPosition = glm::vec4(input->getAxis("horizontal"), 0.0f, input->getAxis("vertical"), 0.0f);

    if (glm::length(deltaPosition) > 0.0f) {
        transform->position += glm::vec3(transform->getRotationMatrix() * deltaPosition) * this->positionSpeed;
    }

    auto deltaShear = glm::vec3(input->getAxis("shorizontal"), 0.0f, input->getAxis("svertical"));

    if (glm::length(deltaShear) > 0.0f) {
        transform->shear += deltaShear * this->positionSpeed;
    }

    auto deltaRotation = glm::vec3(input->getAxis("yaw"), 0.0f, input->getAxis("pitch"));

    if (glm::length(deltaRotation) > 0.0f) {
        transform->deltaRotate(deltaRotation * this->rotationSpeed);
    }

    auto deltaScale = glm::vec3(input->getAxis("scale"));

    if (glm::length(deltaScale) > 0.0f) {
        transform->scale += deltaScale * this->scaleSpeed;
    }

    if(input->getButtonDown("recenter")) {
        transform->position = glm::vec3(rand() % 100 - 50, 0.0f, rand() % 100 - 50);
        transform->setRotation(glm::vec3(0.0f));
        transform->scale = glm::vec3(1.0f);
        transform->shear = glm::vec3(0.0f);
    }
}

void Transformer::imgui() {
    Component::imgui();
    
    ImGui::InputFloat("Position Speed", &this->positionSpeed);
    ImGui::InputFloat("Rotation Speed", &this->rotationSpeed);
    ImGui::InputFloat("Scale Speed", &this->scaleSpeed);
}