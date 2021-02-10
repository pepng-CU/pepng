#pragma once

#include "component.hpp"

class Transformer : public Component {
    public:
        Transformer(float positionSpeed = 0.1f, float rotationSpeed = 1.0f, float scaleSpeed = 0.01f);

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        virtual void imgui() override;
    private:
        float positionSpeed;
        float scaleSpeed;
        float rotationSpeed;
};