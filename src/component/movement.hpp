#pragma once

#include "component.hpp"

class MovementComponent : public Component {
    public:
        MovementComponent(float positionSpeed = 0.1f, float rotationSpeed = 1.0f);

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        virtual void imgui() override;
    private:
        float positionSpeed;
        float rotationSpeed;
};