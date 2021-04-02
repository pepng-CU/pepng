#pragma once

#include <sstream>
#include <vector>
#include <queue>

#include <GL/glew.h>

#include "../component.hpp"

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
        GLenum render_mode;

        /**
         * Does model receive shadow?
         */
        bool receive_shadow;
        
        /**
         * Does model display texture?
         */
        bool display_texture;

        /**
         * Shared_ptr constructor for Selector.
         */
        static std::shared_ptr<Selector> make_selector();

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        #if IMGUI
        virtual void imgui() override;
        #endif

    protected:
        virtual Selector* clone_implementation() override;
        
    private:
        Selector();
        Selector(const Selector& selector);

        /**
         * Selects a child object through DFS.
         * 
         * @deprecated Assignment seems to want the bfs_switch.
         */
        void dfs_switch(std::shared_ptr<Object> object, int& index);

        /**
         * Selects a child object through BFS.
         */
        void bfs_switch(std::shared_ptr<Object> object);

        bool needs_update;
};

namespace pepng {
    std::shared_ptr<Selector> make_selector();
};

#include "../../object/object.hpp"