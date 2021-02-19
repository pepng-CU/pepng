#include "selector.hpp"
#include "../io/io.hpp"
#include "transformer.hpp"
#include "renderer.hpp"

Selector::Selector() : 
    Component("Selector"), 
    index(0), 
    renderMode(GL_TRIANGLES), 
    needsUpdate(true) 
{}

Selector::Selector(const Selector& selector) :
    Component(selector),
    index(selector.index),
    renderMode(selector.renderMode),
    needsUpdate(selector.needsUpdate)
{}

std::shared_ptr<Selector> Selector::makeSelector() {
    std::shared_ptr<Selector> selector(new Selector());

    return selector;
}

std::shared_ptr<Selector> pepng::makeSelector() {
    return Selector::makeSelector();
}

std::shared_ptr<Component> Selector::clone() {
    return this->clone1();
}

std::shared_ptr<Selector> Selector::clone1() {
    std::shared_ptr<Selector> renderer(new Selector(*this));

    return renderer;
}

void Selector::dfsSwitch(std::shared_ptr<Object> object, int& index) {
    try {
        object->getComponent<Transformer>()->isActive = index == this->index;
    } catch(...) {}

    try {
        object->getComponent<Renderer>()->renderMode = this->renderMode;
    } catch(...) {}

    for(auto child : object->children) {
        dfsSwitch(child, ++index);
    }
}

void Selector::update(std::shared_ptr<WithComponents> parent) {
    if (!this->isActive) {
        return;
    }

    auto input = Input::get();

    if(input->getButtonDown("triangles")) {
        this->renderMode = GL_TRIANGLES;
        this->needsUpdate = true;
    } else if(input->getButtonDown("points")) {
        this->renderMode = GL_POINTS;
        this->needsUpdate = true;
    } else if(input->getButtonDown("lines")) {
        this->renderMode = GL_LINES;
        this->needsUpdate = true;
    }

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
    Component::imgui();

    int prev = this->index;

    ImGui::InputInt("Index", &this->index);

    if(prev != this->index) {
        this->needsUpdate = true;
    }

    // TODO: This is identifical to renderer.cpp - must be a way to reuse!
    const char* items[] = { "Lines", "Points", "Triangles" };
    static int item_current_idx = -1;

    if (item_current_idx == -1) {
        switch(this->renderMode) {
            case GL_TRIANGLES:
                item_current_idx = 2;
                break;
            case GL_POINTS:
                item_current_idx = 1;
                break;
            case GL_LINES:
                item_current_idx = 0;
        }
    }

    const char* combo_label = items[item_current_idx];

    if (ImGui::BeginCombo("Render Mode", combo_label)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
            
            if (ImGui::Selectable(items[n], is_selected)) {
                item_current_idx = n;

                switch(item_current_idx) {
                    case 2:
                        this->renderMode = GL_TRIANGLES;
                        break;
                    case 1:
                        this->renderMode = GL_POINTS;
                        break;
                    case 0:
                        this->renderMode = GL_LINES;
                        break;
                }

                this->needsUpdate = true;
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
};