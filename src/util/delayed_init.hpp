#pragma once

#include <vector>
#include <memory>

class DelayedInit {
    public:
        DelayedInit() : isInit(false) {}

        virtual void delayedInit() {
            if(this->isInit) {
                return;
            }

            this->isInit = true;

            for(auto child : this->delayedChildren) {
                child->delayedInit();
            }
        }

        void attachDelayed(std::shared_ptr<DelayedInit> delayedInit) {
            this->delayedChildren.push_back(delayedInit);
        }

    protected:
        bool isInit;

        std::vector<std::shared_ptr<DelayedInit>> delayedChildren;
};  