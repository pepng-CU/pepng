#include "component.hpp"
#include "../io/io.hpp"

Component::Component(std::string name) : 
    name(name),
    isActive(true)
{}

std::string Component::getName() {
    return this->name;
}

void Component::imgui() {
    ImGui::Checkbox("Active", &this->isActive);
}