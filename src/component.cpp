#include "component.hpp"

Component::Component(std::string name) : name(name) {}

std::string Component::getName() {
    return this->name;
}

void ComponentManager::attach(std::shared_ptr<Component> component) {
    this->components.push_back(component);
}

FPSComponent::FPSComponent(float panSpeed, float rotationSpeed) : 
    Component("FPS"),
    panSpeed(panSpeed),
    rotationSpeed(rotationSpeed)
{}

void FPSComponent::update(std::shared_ptr<Transform> parent) {
    auto input = Input::get();
    auto mouseDelta = glm::vec2(input->getAxis("mouseX"), input->getAxis("mouseY"));

    if (input->getButtonDown("pan")) {
        parent->position += parent->getUp() * mouseDelta.y * this->panSpeed + parent->getRight() * mouseDelta.x * this->panSpeed;
    }

    if(input->getButtonDown("rotate")) {
        parent->deltaRotate(glm::vec3(mouseDelta.y, mouseDelta.x, 0.0f) * this->rotationSpeed);
    }

    if(input->getButtonDown("zoom")) {
        parent->position += parent->getForward() * input->getAxis("mouseY") * this->panSpeed;
    }
}

void FPSComponent::imgui() {
    ImGui::InputFloat("panSpeed", &this->panSpeed);
    ImGui::InputFloat("rotationSpeed", &this->rotationSpeed);
}

MovementComponent::MovementComponent(float positionSpeed, float rotationSpeed) :
    Component("Movement"),
    positionSpeed(positionSpeed),
    rotationSpeed(rotationSpeed)
{}

void MovementComponent::update(std::shared_ptr<Transform> parent) {
    auto input = Input::get();

    auto deltaPosition = glm::vec4(input->getAxis("horizontal"), 0.0f, input->getAxis("vertical"), 0.0f);

    if (glm::length(deltaPosition) > 0.0f) {
        parent->position += glm::vec3(parent->getRotationMatrix() * deltaPosition) * this->positionSpeed;
    }

    auto deltaRotation = glm::vec3(input->getAxis("yaw"), 0.0f, input->getAxis("pitch"));

    if (glm::length(deltaRotation) > 0.0f) {
        parent->deltaRotate(glm::normalize(deltaRotation) * this->rotationSpeed);
    }
}

void MovementComponent::imgui() {
    ImGui::InputFloat("Position Speed", &this->positionSpeed);
    ImGui::InputFloat("Rotation Speed", &this->rotationSpeed);
}

RenderModeComponent::RenderModeComponent(std::shared_ptr<GLenum> renderMode) : 
    Component("Render Mode"),
    renderMode(renderMode) 
{}

void RenderModeComponent::update(std::shared_ptr<Transform> parent) {
    auto input = Input::get();

    if(input->getButtonDown("triangles")) {
        *this->renderMode = GL_TRIANGLES;
    } else if(input->getButtonDown("points")) {
        *this->renderMode = GL_POINTS;
    } else if(input->getButtonDown("lines")) {
        *this->renderMode = GL_LINES;
    }
}

void RenderModeComponent::imgui() {
    const char* items[] = { "Triangles", "Points", "Lines" };
    static int item_current_idx = 0;
    const char* combo_label = items[item_current_idx];

    if (ImGui::BeginCombo("Mode", combo_label)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
            
            if (ImGui::Selectable(items[n], is_selected)) {
                item_current_idx = n;

                switch(item_current_idx) {
                    case 0:
                        *this->renderMode = GL_TRIANGLES;
                        break;
                    case 1:
                        *this->renderMode = GL_POINTS;
                        break;
                    case 2:
                        *this->renderMode = GL_LINES;
                        break;
                }
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}