#pragma once

#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "component.hpp"

class Object;

/**
 * Component that select the currently active object.
 */
class Selector : public Component {
    public:
        /**
         * The currently select child (0 is the current parent).
         */
        int index;

        /**
         * The OpenGL render mode that children with renderers will render with.
         */
        GLenum renderMode;

        /**
         * Does model receive shadow?
         */
        bool receiveShadow;
        
        /**
         * Does model display texture?
         */
        bool displayTexture;

        /**
         * Shared_ptr constructor for Selector.
         */
        static std::shared_ptr<Selector> makeSelector();

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        virtual void imgui() override;

    protected:
        virtual Selector* cloneImplementation() override;
        
    private:
        Selector();
        Selector(const Selector& selector);

        void dfsSwitch(std::shared_ptr<Object> object, int& index);

        bool needsUpdate;
};

namespace pepng {
    std::shared_ptr<Selector> makeSelector();
};

#include "../object/object.hpp"