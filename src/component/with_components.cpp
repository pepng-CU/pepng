#include "with_components.hpp"

WithComponents::WithComponents() {}
WithComponents::WithComponents(const WithComponents& withComponents) {
    for(auto component : withComponents.components) {
        this->components.push_back(component->clone());
    }
}

std::shared_ptr<WithComponents> WithComponents::attachComponent(std::shared_ptr<Component> component) {
    this->components.push_back(component);

    component->init(shared_from_this());

    return shared_from_this();
}

void WithComponents::imgui() {
    for(auto component : this->getComponents()) {
        if(ImGui::CollapsingHeader(component->getName().c_str())) {
            component->imgui();
        }
    }
}

void WithComponents::updateComponents() {
    for(auto component : this->getComponents()) {
        component->update(shared_from_this());
    }
}

void WithComponents::renderComponents() {
    for(auto component : this->getComponents()) {
        component->render(shared_from_this());
    }
}

std::ostream& WithComponents::operatorOstream(std::ostream& os) const {
    os << "WithComponents { ";

    for(auto component : this->components) {
        os << *component << ", ";
    }

    os << "}";

    return os;
}

std::ostream& operator<<(std::ostream& os, const WithComponents& component) {
    return component.operatorOstream(os);
}