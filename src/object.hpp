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

class Object : public Transform {
    public:
        std::shared_ptr<Model> model;

        std::vector<std::shared_ptr<Object>> children;

        Object(Transform transform);

        Object(Transform transform, GLuint shaderProgram);

        Object(Transform transform, std::shared_ptr<Model> model, GLuint shaderProgram);

        Object(const Object& object);

        static std::shared_ptr<Object> fromOBJ(std::filesystem::path filepath, GLuint shaderProgram, Transform transform = Transform {});

        virtual void render(std::shared_ptr<Camera> camera, GLenum mode = GL_TRIANGLES, glm::mat4 parentMatrix = glm::mat4(1.0f));

        friend std::ostream& operator<<(std::ostream& os, const Object& object);
        
    protected:
        GLuint shaderProgram;
};

std::ostream& operator<<(std::ostream& os, const Object& object);