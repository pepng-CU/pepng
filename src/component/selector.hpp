#pragma once

#include <sstream>

#include "component.hpp"
#include "movement.hpp"
#include "transform.hpp"

class Object;

class Selector : public Component {
    public:
        int index;

        Selector();

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        virtual void imgui() override;
    private:
        void dfsSwitch(std::shared_ptr<Object> object, int& index);

        bool needsUpdate;
};

#include "../object/object.hpp"