#include "object.hpp"

Object::Object() : Object(Transform {}) {}

Object::Object(Transform transform) : Object(transform, -1) {}

Object::Object(Transform transform, GLuint shaderProgram) : 
    Object(transform, std::make_shared<Model>(), shaderProgram) 
{}

Object::Object(Transform transform, std::shared_ptr<Model> model, GLuint shaderProgram) : 
    Transform(transform), 
    model(model), 
    shaderProgram(shaderProgram) 
{}

Object::Object(const Object& object) : 
    Object(
        Transform(object), 
        std::make_shared<Model>(*object.model), 
        object.shaderProgram
    ) 
{
    std::vector<std::shared_ptr<Object>> children;

    for(auto child : object.children) {
        children.push_back(std::make_shared<Object>(*child));
    }

    this->children = children;
}

std::shared_ptr<Object> Object::fromOBJ(std::filesystem::path filepath, GLuint shaderProgram, Transform transform) {
    std::shared_ptr<Object> object = std::make_shared<Object>(transform);

    for(auto model : Model::fromOBJ(filepath)) {
        object->children.push_back(
            std::make_shared<Object>(
                Transform(transform),
                model,
                shaderProgram
            )
        );
    }

    return object;
}

void Object::render(std::shared_ptr<Camera> camera, GLenum mode, glm::mat4 parentMatrix) {
    auto worldMatrix = this->getWorldMatrix();

    if (this->model->count != -1) {
        glUseProgram(this->shaderProgram);

        camera->render(this->shaderProgram);

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "u_world"),
            1,
            GL_FALSE,
            // Offsets the object to rotate around the object origin 
            // instead of world origin. The object is then put back in place.
            glm::value_ptr(parentMatrix 
                * this->model->getOffsetMatrix() 
                * worldMatrix
                * this->model->getNegativeOffsetMatrix()
            )
        );

        this->model->render(shaderProgram, mode);
    }

    for(auto child : this->children) {
        child->render(camera, mode, parentMatrix * worldMatrix);
    }
}

std::ostream& operator<<(std::ostream& os, const Object& object) {
    os  << "Object { " << (Transform&) object << " }";

    return os;
}