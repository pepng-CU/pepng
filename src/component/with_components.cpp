#include "with_components.hpp"

WithComponents::WithComponents() {}
WithComponents::WithComponents(const WithComponents& withComponents) {
    for(auto component : withComponents.components) {
        this->components.push_back(component->clone());
    }
}

std::shared_ptr<WithComponents> WithComponents::attach_component(std::shared_ptr<Component> component) {
    this->components.push_back(component);

    component->init(shared_from_this());

    return shared_from_this();
}

void WithComponents::imgui() {
    for(auto component : this->get_components()) {
        if(ImGui::CollapsingHeader(component->name().c_str())) {
            component->imgui();
        }
    }
}

void WithComponents::update_components() {
    for(auto component : this->get_components()) {
        component->update(shared_from_this());
    }
}

void WithComponents::render_components() {
    for(auto component : this->get_components()) {
        component->render(shared_from_this());
    }
}

std::ostream& WithComponents::operator_ostream(std::ostream& os) const {
    os << "WithComponents { ";

    for(auto component : this->components) {
        os << *component << ", ";
    }

    os << "}";

    return os;
}

std::ostream& operator<<(std::ostream& os, const WithComponents& component) {
    return component.operator_ostream(os);
}