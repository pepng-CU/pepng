#pragma once

#include <vector>
#include <memory>

#include "cloneable.hpp"

/**
 * Delays the initialization to the main thread.
 */
class DelayedInit : public Cloneable<DelayedInit> {
    public:
        /**
         * Delayed the initialization to the main thread.
         */
        virtual void delayed_init() {
            if(this->_is_init) {
                return;
            }

            this->_is_init = true;

            for(auto child : this->_delayed_children) {
                child->delayed_init();
            }
        }

        /**
         * Attaches child delayed init.
         */
        void attach_delayed(std::shared_ptr<DelayedInit> delayedInit) {
            this->_delayed_children.push_back(delayedInit);
        }

        /**
         * Accessor for init state.
         */
        bool is_init() {
            return this->_is_init;
        }

    protected:
        /**
         * Variable to check if the component is init.
         */
        bool _is_init;

        /**
         * Child delayed components.
         */
        std::vector<std::shared_ptr<DelayedInit>> _delayed_children;

        DelayedInit() : _is_init(false) {}
        DelayedInit(const DelayedInit& delayedInit) : _is_init(delayedInit._is_init) {
            for(auto child : delayedInit._delayed_children) {
                this->_delayed_children.push_back(child->clone());
            }
        }
};  