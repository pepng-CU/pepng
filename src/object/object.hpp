#pragma once

#include <memory>
#include <vector>
#include <filesystem>
#include <iostream>
#include <functional>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../component/transform.hpp"
#include "../component/renderer.hpp"
#include "../util/cloneable.hpp"

/**
 * A generic hold of components with child/parent relationship.
 */
class Object : public WithComponents, public Cloneable<Object> {
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
        static std::shared_ptr<Object> make_object(std::string name);

        /**
         * Attaches a child to this object.
         */
        std::shared_ptr<Object> attach_child(std::shared_ptr<Object> object);

        /**
         * Applies callback function of this object and all children object.
         */
        void for_each(std::function<void (std::shared_ptr<Object>)> callback);

        virtual void update();

        virtual void render();

        void render(GLuint shaderProgram);

        virtual std::ostream& operator_ostream(std::ostream& os) const override;

        friend std::ostream& operator<<(std::ostream& os, const Object& object);

        #if IMGUI
        virtual void imgui();
        #endif

    protected:
        Object(std::string name);
        Object(const Object& object);

        virtual Object* clone_implementation() override;
};

namespace pepng {
    std::shared_ptr<Object> make_object(std::string name);
};

std::ostream& operator<<(std::ostream& os, const Object& object);