#include "with_components.hpp"

void WithComponents::attach(std::shared_ptr<Component> component) {
    this->components.push_back(component);
}

void WithComponents::imgui() {
    for(auto component : this->getComponents()) {
        if(ImGui::CollapsingHeader(component->getName().c_str())) {
            component->imgui();
        }
    }
}