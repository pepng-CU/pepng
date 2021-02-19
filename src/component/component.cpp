#include "component.hpp"

Component::Component(std::string name) : 
    name(name),
    isActive(true)
{}

Component::Component(const Component& component) : 
    name(component.name),
    isActive(component.isActive)
{}

std::string Component::getName() {
    return this->name;
}

void Component::imgui() {
    ImGui::Checkbox("Active", &this->isActive);
}

std::ostream& Component::operatorOstream(std::ostream& os) const {
    os << "Component { " << this->name << " }";

    return os;
}

std::ostream& operator<<(std::ostream& os, const Component& component) {
    return component.operatorOstream(os);
}