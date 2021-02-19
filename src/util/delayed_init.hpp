#pragma once

#include <vector>
#include <memory>

/**
 * Delays the initialization to the main thread.
 */
class DelayedInit {
    public:
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

        virtual std::shared_ptr<DelayedInit> clone() = 0;

    protected:
        /**
         * Variable to check if the component is init.
         */
        bool isInit;

        /**
         * Child delayed components.
         */
        std::vector<std::shared_ptr<DelayedInit>> delayedChildren;

        DelayedInit() : isInit(false) {}
        DelayedInit(const DelayedInit& delayedInit) : isInit(delayedInit.isInit) {
            for(auto child : delayedInit.delayedChildren) {
                this->delayedChildren.push_back(child->clone());
            }
        }
};  