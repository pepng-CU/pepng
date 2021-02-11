#include "object.hpp"

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

std::shared_ptr<Object> Object::fromOBJ(std::filesystem::path filepath, GLuint shaderProgram, std::shared_ptr<Transform> transform) {
    std::shared_ptr<Object> object = pepng::makeObject(filepath.filename());

    object
        ->attachComponent(pepng::copyTransform(transform));

    for(auto model : Model::fromOBJ(filepath)) {
        auto child = pepng::makeObject(model->getName());

        child
            ->attachComponent(pepng::makeTransform())
            ->attachComponent(pepng::makeRenderer(model, shaderProgram, -1, GL_TRIANGLES));

        object->children.push_back(child);
    }

    return object;
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