#pragma once

#include "component.hpp"

/**
 * Component that applies IO based transformation on Transform.
 */
class Transformer : public Component {
    public:
        static std::shared_ptr<Transformer> makeTransformer(float positionSpeed = 0.1f, float rotationSpeed = 1.0f, float scaleSpeed = 0.01f);

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        virtual void imgui() override;
    private:
        Transformer(float positionSpeed, float rotationSpeed, float scaleSpeed);

        /**
         * The translation speed to apply.
         */
        float positionSpeed;
        /**
         * The scale speed to apply.
         */
        float scaleSpeed;
        /**
         * The rotation speed to apply.
         */
        float rotationSpeed;
};

namespace pepng {
    std::shared_ptr<Transformer> makeTransformer(float positionSpeed = 0.1f, float rotationSpeed = 1.0f, float scaleSpeed = 0.01f);
};