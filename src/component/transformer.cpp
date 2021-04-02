#include "transformer.hpp"

#include "../io/io.hpp"
#include "transform.hpp"

Transformer::Transformer(float positionSpeed, float rotationSpeed, float scaleSpeed) :
    Component("Transformer"),
    __position_speed(positionSpeed),
    __rotation_speed(rotationSpeed),
    __scale_speed(scaleSpeed)
{}

Transformer::Transformer(const Transformer& transformer) :
    Component(transformer),
    __position_speed(transformer.__position_speed),
    __rotation_speed(transformer.__rotation_speed),
    __scale_speed(transformer.__scale_speed)
{}

std::shared_ptr<Transformer> Transformer::make_transformer(float positionSpeed, float rotationSpeed, float scaleSpeed) {
    std::shared_ptr<Transformer> transformer(new Transformer(positionSpeed, rotationSpeed, scaleSpeed));

    return transformer;
}

std::shared_ptr<Transformer> pepng::make_transformer(float positionSpeed, float rotationSpeed, float scaleSpeed) {
    return Transformer::make_transformer(positionSpeed, rotationSpeed, scaleSpeed);
}

Transformer* Transformer::clone_implementation() {
    return new Transformer(*this);
}

void Transformer::update(std::shared_ptr<WithComponents> parent) {
    if(!this->_is_active) {
        return;
    }

    auto transform = parent->get_component<Transform>();

    auto input = Input::get();

    auto deltaPosition = glm::vec4(input->axis("horizontal"), 0.0f, input->axis("vertical"), 0.0f);

    if (glm::length(deltaPosition) > 0.0f) {
        transform->position += glm::vec3(transform->rotation_matrix() * deltaPosition) * this->__position_speed;
    }

    auto deltaShear = glm::vec3(input->axis("shorizontal"), 0.0f, input->axis("svertical"));

    if (glm::length(deltaShear) > 0.0f) {
        transform->shear += deltaShear * this->__position_speed;
    }

    auto deltaRotation = glm::vec3(input->axis("yaw"), 0.0f, input->axis("pitch"));

    if (glm::length(deltaRotation) > 0.0f) {
        transform->delta_rotate(deltaRotation * this->__rotation_speed);
    }

    auto deltaScale = glm::vec3(input->axis("scale"));

    if (glm::length(deltaScale) > 0.0f) {
        transform->scale += deltaScale * this->__scale_speed;
    }

    if(input->button_down("recenter")) {
        transform->position = glm::vec3(rand() % 100 - 50, 0.0f, rand() % 100 - 50);
        transform->set_rotation(glm::vec3(0.0f));
        transform->scale = glm::vec3(1.0f);
        transform->shear = glm::vec3(0.0f);
    }
}

void Transformer::imgui() {
    Component::imgui();
    
    ImGui::InputFloat("Position Speed", &this->__position_speed);
    ImGui::InputFloat("Rotation Speed", &this->__rotation_speed);
    ImGui::InputFloat("Scale Speed", &this->__scale_speed);
}