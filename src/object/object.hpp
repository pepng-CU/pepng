#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <filesystem>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "../component/transform.hpp"
#include "../component/renderer.hpp"

class Object : public WithComponents {
    public:
        std::string name;

        std::vector<std::shared_ptr<Object>> children;

        static std::shared_ptr<Object> makeObject(std::string name);

        std::shared_ptr<Object> attachChild(std::shared_ptr<Object> object);

        virtual void imgui();

        virtual void update();

        virtual void render();

        void render(GLuint shaderProgram);

        virtual std::ostream& operatorOstream(std::ostream& os) const override;

        friend std::ostream& operator<<(std::ostream& os, const Object& object);

    protected:
        Object(std::string name);
};

namespace pepng {
    std::shared_ptr<Object> makeObject(std::string name);
};

std::ostream& operator<<(std::ostream& os, const Object& object);