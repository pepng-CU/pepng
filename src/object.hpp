#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <filesystem>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "transform.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "component.hpp"

class Object : public Transform, public ComponentManager, public std::enable_shared_from_this<Object> {
    public:
        std::shared_ptr<Model> model;

        std::vector<std::shared_ptr<Object>> children;

        Object();

        Object(Transform transform);

        Object(Transform transform, GLuint shaderProgram);

        Object(Transform transform, std::shared_ptr<Model> model, GLuint shaderProgram);

        Object(const Object& object);

        static std::shared_ptr<Object> fromOBJ(std::filesystem::path filepath, GLuint shaderProgram, Transform transform = Transform {});

        virtual void render(std::shared_ptr<Camera> camera, GLenum mode = GL_TRIANGLES, glm::mat4 parentMatrix = glm::mat4(1.0f));

        virtual void update() override {
            for(auto component : this->components) {
                component->update(shared_from_this());
            }
        }

        std::shared_ptr<Object> attachChild(std::shared_ptr<Object> object);

        friend std::ostream& operator<<(std::ostream& os, const Object& object);
        
    protected:
        GLuint shaderProgram;
};

std::ostream& operator<<(std::ostream& os, const Object& object);