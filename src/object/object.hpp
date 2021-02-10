#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <filesystem>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "../component/components.hpp"

class Object : public WithComponents, public std::enable_shared_from_this<Object> {
    public:
        std::string name;

        std::vector<std::shared_ptr<Object>> children;

        Object();

        Object(std::string name, std::shared_ptr<Transform> transform);

        static std::shared_ptr<Object> fromOBJ(std::filesystem::path filepath, GLuint shaderProgram, std::shared_ptr<Transform> transform);

        virtual void imgui();

        virtual void update() override;

        std::shared_ptr<Object> attachChild(std::shared_ptr<Object> object);
};