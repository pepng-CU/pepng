#pragma once

#include "component.hpp"

class FPSComponent : public Component {
    public:
        FPSComponent(float panSpeed = 0.025f, float rotationSpeed = 0.25f);

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        virtual void imgui() override;
    private:
        float panSpeed;
        float rotationSpeed;
};