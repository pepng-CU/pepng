#pragma once

#include "component.hpp"

/**
 * Component that applies IO based transformation on Transform.
 */
class Transformer : public Component {
    public:
        static std::shared_ptr<Transformer> make_transformer(float positionSpeed = 0.1f, float rotationSpeed = 1.0f, float scaleSpeed = 0.01f);

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        virtual void imgui() override;

    protected:
        virtual Transformer* clone_implementation() override;

    private:
        Transformer(float positionSpeed, float rotationSpeed, float scaleSpeed);
        Transformer(const Transformer& transformer);

        /**
         * The translation speed to apply.
         */
        float __position_speed;
        /**
         * The scale speed to apply.
         */
        float __scale_speed;
        /**
         * The rotation speed to apply.
         */
        float __rotation_speed;
};

namespace pepng {
    std::shared_ptr<Transformer> make_transformer(float positionSpeed = 0.1f, float rotationSpeed = 1.0f, float scaleSpeed = 0.01f);
};