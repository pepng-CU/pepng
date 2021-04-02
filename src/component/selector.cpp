#include "selector.hpp"

#include "../io/io.hpp"
#include "transformer.hpp"
#include "renderer.hpp"

Selector::Selector() : 
    Component("Selector"), 
    index(0), 
    render_mode(GL_TRIANGLES), 
    needs_update(true),
    receive_shadow(true),
    display_texture(true)
{}

Selector::Selector(const Selector& selector) :
    Component(selector),
    index(selector.index),
    render_mode(selector.render_mode),
    needs_update(selector.needs_update),
    receive_shadow(selector.receive_shadow),
    display_texture(selector.display_texture)
{}

std::shared_ptr<Selector> Selector::make_selector() {
    std::shared_ptr<Selector> selector(new Selector());

    return selector;
}

std::shared_ptr<Selector> pepng::make_selector() {
    return Selector::make_selector();
}

Selector* Selector::clone_implementation() {
    return new Selector(*this);
}

void Selector::dfs_switch(std::shared_ptr<Object> object, int& index) {
    try {
        object->get_component<Transformer>()->set_active(index == this->index);
    } catch(...) {}

    try {
        auto renderer = object->get_component<Renderer>();

        renderer->render_mode = this->render_mode;
        renderer->receive_shadow = this->receive_shadow;
        renderer->display_texture = this->display_texture;
    } catch(...) {}

    for(auto child : object->children) {
        dfs_switch(child, ++index);
    }
}

void Selector::bfs_switch(std::shared_ptr<Object> object) {
    int index = 0;

    std::queue<std::shared_ptr<Object>> currentQueue;

    currentQueue.push(object);

    std::queue<std::shared_ptr<Object>> nextQueue;

    while(true) {
        while(!currentQueue.empty()) {
            auto object = currentQueue.front();
            currentQueue.pop();

            for(auto child : object->children) {
                nextQueue.push(child);
            }
            
            try {
                object->get_component<Transformer>()->set_active(index++ == this->index);
            } catch(...) {}

            try {
                auto renderer = object->get_component<Renderer>();

                renderer->render_mode = this->render_mode;
                renderer->receive_shadow = this->receive_shadow;
                renderer->display_texture = this->display_texture;
            } catch(...) {}
        }

        if(nextQueue.empty()) break;

        currentQueue = nextQueue;
        nextQueue = std::queue<std::shared_ptr<Object>>();
    }
}

void Selector::update(std::shared_ptr<WithComponents> parent) {
    if (!this->_is_active) return;

    auto input = Input::get();

    if(input->button_down("triangles")) {
        this->render_mode = GL_TRIANGLES;
        this->needs_update = true;
    } else if(input->button_down("points")) {
        this->render_mode = GL_POINTS;
        this->needs_update = true;
    } else if(input->button_down("lines")) {
        this->render_mode = GL_LINES;
        this->needs_update = true;
    }

    if(input->button_down("shadow")) {
        this->receive_shadow = !this->receive_shadow;
        this->needs_update = true;
    }

    if(input->button_down("texture")) {
        this->display_texture = !this->display_texture;
        this->needs_update = true;
    }

    if (this->needs_update) {
        this->needs_update = false;

        if(auto object = std::dynamic_pointer_cast<Object>(parent)) {
            bfs_switch(object);
        }
    }

    for(int i = 0; i < 10; i++) {
        std::stringstream ss;

        ss << "object_" << i;

        if (input->button(ss.str())) {
            this->index = i;

            this->needs_update = true;
        }
    }
};

void Selector::imgui() {
    Component::imgui();

    int prev = this->index;

    ImGui::InputInt("Index", &this->index);

    if(prev != this->index) {
        this->needs_update = true;
    }

    // TODO: This is identifical to renderer.cpp - must be a way to reuse!
    const char* items[] = { "Lines", "Points", "Triangles" };
    static int item_current_idx = -1;

    if (item_current_idx == -1) {
        switch(this->render_mode) {
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
                        this->render_mode = GL_TRIANGLES;
                        break;
                    case 1:
                        this->render_mode = GL_POINTS;
                        break;
                    case 0:
                        this->render_mode = GL_LINES;
                        break;
                }

                this->needs_update = true;
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::Checkbox("Texture", &this->display_texture)) this->needs_update = true;
    if (ImGui::Checkbox("Shadow", &this->receive_shadow)) this->needs_update = true;
};