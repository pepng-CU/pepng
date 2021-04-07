#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <GL/glew.h>

#ifdef IMGUI
#include <imgui.h>
#include "../ui/with_imgui.hpp"
#endif

#include "../util/cloneable.hpp"

class WithComponents;

/**
 * Abstract Component definition that is binded to an object/component holder.
 */
class Component : 
    #ifdef IMGUI
    public WithImGui, 
    #endif
    public Cloneable<Component> 
{
    public:
        /**
         * Accessor for the component name.
         */
        inline std::string name() { return this->__name; }

        /**
         * Accessor for component active state.
         */
        inline bool active() { return this->_is_active; }

        /**
         * Mutator for component active state.
         */
        inline void set_active(bool active) { this->_is_active = active; }

        /**
         * Virtual operator<< to allow for child class redefine.
         */
        virtual std::ostream& operator_ostream(std::ostream& os) const;

        friend std::ostream& operator<<(std::ostream& os, const Component& component);

        virtual void init(std::shared_ptr<WithComponents> parent) {};

        virtual void render(std::shared_ptr<WithComponents> parent) {};

        virtual void update(std::shared_ptr<WithComponents> parent) {};

        #ifdef IMGUI
        virtual void imgui() override;
        #endif

    protected:
        Component(std::string name);

        Component(const Component& component);

        /**
         * Used to check if this component will update.
         */
        bool _is_active;

    private:
        /**
         * Name of the component (the child class will define this in the constructor).
         */
        std::string __name;
};

std::ostream& operator<<(std::ostream& os, const Component& component);

#include "with_components.hpp"