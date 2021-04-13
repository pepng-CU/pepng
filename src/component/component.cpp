#include "component.hpp"

Component::Component(std::string name) : 
    _name(name),
    _is_active(true)
{}

Component::Component(const Component& component) : 
    _name(component._name),
    _is_active(component._is_active)
{}

std::ostream& Component::operator_ostream(std::ostream& os) const {
    os << "Component { " << this->_name << " }";

    return os;
}

std::ostream& operator<<(std::ostream& os, const Component& component) {
    return component.operator_ostream(os);
}

#ifdef IMGUI
void Component::imgui() {
    ImGui::Checkbox("Active", &this->_is_active);
}
#endif