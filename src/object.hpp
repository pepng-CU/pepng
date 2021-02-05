#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <filesystem>
#include <glm/gtx/string_cast.hpp>

#include "transform.hpp"
#include "model.hpp"
#include "camera.hpp"

class Object {
    public:
        Transform transform;
        std::shared_ptr<Model> model;
        GLuint shaderProgram;
        std::vector<std::shared_ptr<Object>> children;

        Object(Transform transform);

        Object(Transform transform, std::shared_ptr<Model> model, GLuint shaderProgram);

        static std::shared_ptr<Object> fromOBJ(std::filesystem::path filepath, GLuint shaderProgram, Transform transform = Transform {});

        // void render(std::shared_ptr<Camera> camera, GLenum mode = GL_TRIANGLES, Transform parentTransform = Transform {});

        void render(std::shared_ptr<Camera> camera, GLenum mode = GL_TRIANGLES, glm::mat4 parentMatrix = glm::mat4(1.0f));
};