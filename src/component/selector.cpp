#include "selector.hpp"

Selector::Selector() : Component("Selector"), index(0), needsUpdate(true) {}

void Selector::dfsSwitch(std::shared_ptr<Object> object, int& index) {
    if(auto move = object->getComponent<Transformer>()) {
        move->isActive = index == this->index;
    }

    for(auto child : object->children) {
        dfsSwitch(child, ++index);
    }
}

void Selector::update(std::shared_ptr<WithComponents> parent) {
    if (!this->isActive) {
        return;
    }

    auto input = Input::get();

    if (this->needsUpdate) {
        this->needsUpdate = false;

        if(auto object = std::dynamic_pointer_cast<Object>(parent)) {
            int index = 0;

            dfsSwitch(object, index);
        }
    }

    for(int i = 0; i < 10; i++) {
        std::stringstream ss;

        ss << "object_" << i;

        if (input->getButtonDown(ss.str())) {
            this->index = i;

            this->needsUpdate = true;
        }
    }
};

void Selector::imgui() {
    int prev = this->index;

    ImGui::InputInt("Index", &this->index);

    if(prev != this->index) {
        this->needsUpdate = true;
    }
};