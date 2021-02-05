#include "object.hpp"

Object::Object(Transform transform) : Object(transform, nullptr, -1) {}

Object::Object(Transform transform, std::shared_ptr<Model> model, GLuint shaderProgram) : transform(transform), model(model), shaderProgram(shaderProgram) {}

std::shared_ptr<Object> Object::fromOBJ(std::filesystem::path filepath, GLuint shaderProgram, Transform transform) {
    std::shared_ptr<Object> object = std::make_shared<Object>(transform);

    for(auto model : Model::fromOBJ(filepath)) {
        object->children.push_back(
            std::make_shared<Object>(
                Transform {},
                model,
                shaderProgram
            )
        );
    }

    return object;
}

void Object::render(std::shared_ptr<Camera> camera, GLenum mode, glm::mat4 parentMatrix) {
    auto worldMatrix = this->transform.getWorldMatrix();

    if (this->model != nullptr) {
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