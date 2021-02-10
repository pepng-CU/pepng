#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <filesystem>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "../components/transform.hpp"
#include "../components/model.hpp"
#include "../components/component.hpp"

class Object : public Transform, public ComponentManager, public std::enable_shared_from_this<Object> {
    public:
        std::string name;

        std::vector<std::shared_ptr<Object>> children;

        Object();

        Object(std::string name, Transform transform);

        Object(const Object& object);

        static std::shared_ptr<Object> fromOBJ(std::filesystem::path filepath, GLuint shaderProgram, Transform transform = Transform {});

        virtual void imgui();

        virtual void update() override;

        std::shared_ptr<Object> attachChild(std::shared_ptr<Object> object);

        friend std::ostream& operator<<(std::ostream& os, const Object& object);
};

std::ostream& operator<<(std::ostream& os, const Object& object);