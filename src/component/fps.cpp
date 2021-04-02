#include "fps.hpp"

#include "../io/io.hpp"
#include "transform.hpp"

FPS::FPS(float panSpeed, float rotationSpeed) : 
    Component("FPS"),
    __pan_speed(panSpeed),
    __rotation_speed(rotationSpeed)
{}

FPS::FPS(const FPS& fps) : 
    Component(fps),
    __pan_speed(fps.__pan_speed),
    __rotation_speed(fps.__rotation_speed)
{}

std::shared_ptr<FPS> FPS::make_fps(float panSpeed, float rotationSpeed) {
    std::shared_ptr<FPS> fps(new FPS(panSpeed, rotationSpeed));

    return fps;
}

std::shared_ptr<FPS> pepng::make_fps(float panSpeed, float rotationSpeed) {
    return FPS::make_fps(panSpeed, rotationSpeed);
}

FPS* FPS::clone_implementation() {
    return new FPS(*this);
}

void FPS::update(std::shared_ptr<WithComponents> parent) {
    if(!this->_is_active) {
        return;
    }

    auto transform = parent->get_component<Transform>();

    auto input = Input::get();
    auto mouseDelta = glm::vec2(input->axis("mouseX"), input->axis("mouseY"));

    if (input->button("pan")) {
        transform->position += transform->up() * mouseDelta.y * this->__pan_speed + transform->right() * mouseDelta.x * this->__pan_speed;
    }

    auto rotation = glm::vec3(mouseDelta.y, mouseDelta.x, 0.0f);

    if(input->button("rotate") && glm::length(rotation) > 0.25f) {
        transform->delta_rotate(rotation * this->__rotation_speed);
    }

    transform->position -= transform->forward() * input->axis("zoom") * this->__pan_speed;
}

void FPS::imgui() {
    Component::imgui();

    ImGui::InputFloat("Pan Speed", &this->__pan_speed);
    ImGui::InputFloat("Rotation Speed", &this->__rotation_speed);
}