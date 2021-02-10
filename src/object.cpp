#include "object.hpp"

Object::Object() : 
    Object("Object", Transform {}) 
{}

Object::Object(std::string name, Transform transform) :
    Transform(transform),
    name(name)
{}

Object::Object(const Object& object) : 
    Transform(object)
{
    std::vector<std::shared_ptr<Object>> children;

    for(auto child : object.children) {
        children.push_back(std::make_shared<Object>(*child));
    }

    this->children = children;
}

std::shared_ptr<Object> Object::attachChild(std::shared_ptr<Object> object) {
    this->children.push_back(object);

    return shared_from_this();
}

std::shared_ptr<Object> Object::fromOBJ(std::filesystem::path filepath, GLuint shaderProgram, Transform transform) {
    std::shared_ptr<Object> object = std::make_shared<Object>(filepath.filename(), transform);

    for(auto model : Model::fromOBJ(filepath, shaderProgram)) {
        auto child = std::make_shared<Object>(model->name, Transform(transform));

        child->attach(model);

        object->children.push_back(child);
    }

    return object;
}

void Object::imgui() {
    ImGui::LabelText("Name", this->name.c_str());

    Transform::imgui();

    ComponentManager::imgui();
}

void Object::update(){
    auto parentMatrix = this->parentMatrix * this->getWorldMatrix();

    for(auto component : this->getComponents()) {
        component->update(shared_from_this());
    }

    for(auto child : this->children) {
        child->parentMatrix = parentMatrix;

        child->update();
    }
}

std::ostream& operator<<(std::ostream& os, const Object& object) {
    os  << "Object { " 
        << (Transform&) object 
        << " }";

    return os;
}