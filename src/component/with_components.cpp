#include "with_components.hpp"

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