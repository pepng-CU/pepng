#include "component.hpp"

Component::Component(std::string name) : 
    __name(name),
    _is_active(true)
{}

Component::Component(const Component& component) : 
    __name(component.__name),
    _is_active(component._is_active)
{}

std::ostream& Component::operator_ostream(std::ostream& os) const {
    os << "Component { " << this->__name << " }";

    return os;
}

std::ostream& operator<<(std::ostream& os, const Component& component) {
    return component.operator_ostream(os);
}

#if IMGUI
void Component::imgui() {
    ImGui::Checkbox("Active", &this->_is_active);
}
#endif