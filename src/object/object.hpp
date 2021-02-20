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

/**
 * A generic hold of components with child/parent relationship.
 */
class Object : public WithComponents {
    public:
        /**
         * The name of the object.
         */
        std::string name;

        /**
         * The children attached to this object.
         */
        std::vector<std::shared_ptr<Object>> children;

        /**
         * Shared_ptr constructor for Object.
         */
        static std::shared_ptr<Object> makeObject(std::string name);

        virtual std::shared_ptr<Object> clone();

        /**
         * Attaches a child to this object.
         */
        std::shared_ptr<Object> attachChild(std::shared_ptr<Object> object);

        virtual void imgui();

        virtual void update();

        virtual void render();

        void render(GLuint shaderProgram);

        virtual std::ostream& operatorOstream(std::ostream& os) const override;

        friend std::ostream& operator<<(std::ostream& os, const Object& object);

    protected:
        Object(std::string name);
        Object(const Object& object);
};

namespace pepng {
    std::shared_ptr<Object> makeObject(std::string name);
};

std::ostream& operator<<(std::ostream& os, const Object& object);