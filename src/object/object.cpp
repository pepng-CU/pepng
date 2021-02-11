#include "object.hpp"
#include "../component/renderer.hpp"

Object::Object(std::string name) : 
    name(name) 
{}

std::shared_ptr<Object> Object::makeObject(std::string name) {
    std::shared_ptr<Object> object(new Object(name));

    return object;
}

std::shared_ptr<Object> Object::attachChild(std::shared_ptr<Object> object) {
    this->children.push_back(object);

    return std::dynamic_pointer_cast<Object>(shared_from_this());
}

void Object::imgui() {
    ImGui::LabelText("Name", this->name.c_str());

    WithComponents::imgui();
}

void Object::update() {
    WithComponents::updateComponents();

    auto transform = this->getComponent<Transform>();

    auto parentMatrix = transform->parentMatrix * transform->getWorldMatrix();

    for(auto child : this->children) {
        auto childTransform = child->getComponent<Transform>();

        childTransform->parentMatrix = glm::mat4(parentMatrix);

        child->update();
    }
}

namespace pepng {
    std::shared_ptr<Object> makeObject(std::string name) {
        return Object::makeObject(name);
    }
};