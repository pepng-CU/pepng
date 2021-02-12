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

    if(transform == nullptr) {
        std::stringstream ss;

        ss << *this << " has no transform.";

        throw std::runtime_error(ss.str());
    }

    auto parentMatrix = transform->parentMatrix * transform->getWorldMatrix();

    for(auto child : this->children) {
        if(child == nullptr) {
            continue;
        }

        if(auto childTransform = child->getComponent<Transform>()) {
            childTransform->parentMatrix = glm::mat4(parentMatrix);
        }

        child->update();
    }
}

void Object::render() {
    WithComponents::renderComponents();

    for(auto child : this->children) {
        child->render();
    }
}

std::ostream& Object::operatorOstream(std::ostream& os) const {
    os  << "Object { " 
        << this->name
        << ", "; 

    WithComponents::operatorOstream(os);

    os << " }";

    return os;
}

std::ostream& operator<<(std::ostream& os, const Object& object) {
    return object.operatorOstream(os);
}

namespace pepng {
    std::shared_ptr<Object> makeObject(std::string name) {
        return Object::makeObject(name);
    }
};