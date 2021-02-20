#pragma once

#include <memory>
#include <vector>
#include <imgui.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <string>

#include "../ui/with_imgui.hpp"

class WithComponents;

/**
 * Abstract Component definition that is binded to an object/component holder.
 */
class Component : public WithImGui {
    public:
        /**
         * Used to check if this component will update.
         */
        bool isActive;

        /**
         * Accessor for the component name.
         */
        std::string getName();

        /**
         * Virtual operator<< to allow for child class redefine.
         */
        virtual std::ostream& operatorOstream(std::ostream& os) const;

        friend std::ostream& operator<<(std::ostream& os, const Component& component);

        virtual void init(std::shared_ptr<WithComponents> parent) {};

        virtual void render(std::shared_ptr<WithComponents> parent) {};

        virtual void update(std::shared_ptr<WithComponents> parent) {};

        virtual void imgui() override;

        virtual std::shared_ptr<Component> clone() = 0;

    protected:
        Component(std::string name);

        Component(const Component& component);

    private:
        /**
         * Name of the component (the child class will define this in the constructor).
         */
        std::string name;
};

std::ostream& operator<<(std::ostream& os, const Component& component);

#include "with_components.hpp"