#pragma once

#include "component.hpp"

class FPS : public Component {
    public:
        static std::shared_ptr<FPS> makeFPS(float panSpeed, float rotationSpeed);

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        virtual void imgui() override;
    private:
        FPS(float panSpeed, float rotationSpeed);

        float panSpeed;
        float rotationSpeed;
};

namespace pepng {
    std::shared_ptr<FPS> makeFPS(float panSpeed = 0.025f, float rotationSpeed = 0.5f);
}