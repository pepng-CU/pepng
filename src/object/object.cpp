#include "object.hpp"
#include "../component/renderer.hpp"

Object::Object(std::string name) : 
    WithComponents(),
    name(name)
{}

Object::Object(const Object& object) : 
    WithComponents(object),
    name(object.name)
{
    for(auto child : object.children) {
        this->children.push_back(child->clone());
    }
}

std::shared_ptr<Object> Object::make_object(std::string name) {
    std::shared_ptr<Object> object(new Object(name));

    return object;
}

Object* Object::clone_implementation() {
    return new Object(*this);
}

std::shared_ptr<Object> Object::attach_child(std::shared_ptr<Object> object) {
    this->children.push_back(object);

    return std::dynamic_pointer_cast<Object>(shared_from_this());
}

void Object::update() {
    WithComponents::update_components();

    auto transform = this->get_component<Transform>();

    if(transform == nullptr) {
        std::stringstream ss;

        ss << *this << " has no transform.";

        std::cout << ss.str() << std::endl;

        throw std::runtime_error(ss.str());
    }

    auto parent_matrix = transform->parent_matrix * transform->world_matrix();

    for(auto child : this->children) {
        if(child == nullptr) {
            continue;
        }

        if(auto childTransform = child->get_component<Transform>()) {
            childTransform->parent_matrix = glm::mat4(parent_matrix);
        }

        child->update();
    }
}

void Object::render(GLuint shaderProgram) {
    for(auto component : this->get_components()) {
        if(auto renderer = std::dynamic_pointer_cast<Renderer>(component)) {
            renderer->render(shared_from_this(), shaderProgram);
        }
    }

    for(auto child : this->children) {
        child->render(shaderProgram);
    }
}

void Object::render() {
    WithComponents::render_components();

    for(auto child : this->children) {
        child->render();
    }
}

std::ostream& Object::operator_ostream(std::ostream& os) const {
    os  << "Object { " 
        << this->name
        << ", "; 

    WithComponents::operator_ostream(os);

    os << " }";

    return os;
}

void Object::for_each(std::function<void (std::shared_ptr<Object>)> callback) {
    callback(std::static_pointer_cast<Object>(shared_from_this()));

    for(auto child : this->children) {
        child->for_each(callback);
    }
}

std::ostream& operator<<(std::ostream& os, const Object& object) {
    return object.operator_ostream(os);
}

namespace pepng {
    std::shared_ptr<Object> make_object(std::string name) {
        return Object::make_object(name);
    }
}

#if IMGUI
void Object::imgui() {
    ImGui::LabelText("Name", this->name.c_str());

    WithComponents::imgui();
}
#endif