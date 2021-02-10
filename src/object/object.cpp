#include "object.hpp"

Object::Object() : 
    Object("Object", std::make_shared<Transform>()) 
{}

Object::Object(std::string name, std::shared_ptr<Transform> transform) :
    name(name)
{
    this->attach(transform);
}

std::shared_ptr<Object> Object::attachChild(std::shared_ptr<Object> object) {
    this->children.push_back(object);

    return shared_from_this();
}

std::shared_ptr<Object> Object::fromOBJ(std::filesystem::path filepath, GLuint shaderProgram, std::shared_ptr<Transform> transform) {
    std::shared_ptr<Object> object = std::make_shared<Object>(filepath.filename(), std::make_shared<Transform>(*transform));

    for(auto model : Model::fromOBJ(filepath)) {
        auto child = std::make_shared<Object>(model->getName(), std::make_shared<Transform>());

        child->attach(std::make_shared<Renderer>(model, shaderProgram, -1, GL_TRIANGLES));

        object->children.push_back(child);
    }

    return object;
}

void Object::imgui() {
    ImGui::LabelText("Name", this->name.c_str());

    WithComponents::imgui();
}

void Object::update(){
    for(auto component : this->getComponents()) {
        component->update(shared_from_this());
    }

    auto transform = this->getComponent<Transform>();

    auto parentMatrix = transform->parentMatrix * transform->getWorldMatrix();

    for(auto child : this->children) {
        auto childTransform = child->getComponent<Transform>();

        childTransform->parentMatrix = glm::mat4(parentMatrix);

        child->update();
    }
}

std::ostream& operator<<(std::ostream& os, const Object& object) {
    os  << "Object { " 
        << (Transform&) object 
        << " }";

    return os;
}