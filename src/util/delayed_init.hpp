#pragma once

#include <vector>
#include <memory>

/**
 * Delays the initialization to the main thread.
 */
class DelayedInit {
    public:
        DelayedInit() : isInit(false) {}

        /**
         * Delayed the initialization to the main thread.
         */
        virtual void delayedInit() {
            if(this->isInit) {
                return;
            }

            this->isInit = true;

            for(auto child : this->delayedChildren) {
                child->delayedInit();
            }
        }

        /**
         * Attaches child delayed init.
         */
        void attachDelayed(std::shared_ptr<DelayedInit> delayedInit) {
            this->delayedChildren.push_back(delayedInit);
        }

    protected:
        /**
         * Variable to check if the component is init.
         */
        bool isInit;

        /**
         * Child delayed components.
         */
        std::vector<std::shared_ptr<DelayedInit>> delayedChildren;
};  