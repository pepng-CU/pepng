#pragma once

#include "component.hpp"

class Transformer : public Component {
    public:
        static std::shared_ptr<Transformer> makeTransformer(float positionSpeed = 0.1f, float rotationSpeed = 1.0f, float scaleSpeed = 0.01f);

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        virtual void imgui() override;
    private:
        Transformer(float positionSpeed, float rotationSpeed, float scaleSpeed);

        float positionSpeed;
        float scaleSpeed;
        float rotationSpeed;
};

namespace pepng {
    std::shared_ptr<Transformer> makeTransformer(float positionSpeed = 0.1f, float rotationSpeed = 1.0f, float scaleSpeed = 0.01f);
};