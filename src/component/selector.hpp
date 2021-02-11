#pragma once

#include <sstream>

#include "component.hpp"

class Object;

class Selector : public Component {
    public:
        int index;

        static std::shared_ptr<Selector> makeSelector();

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        virtual void imgui() override;
    private:
        Selector();

        void dfsSwitch(std::shared_ptr<Object> object, int& index);

        bool needsUpdate;
};

namespace pepng {
    std::shared_ptr<Selector> makeSelector();
};

#include "../object/object.hpp"