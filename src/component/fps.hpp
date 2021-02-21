#pragma once

#include "component.hpp"

/**
 * An FPS like controller for object.
 */
class FPS : public Component {
    public:
        static std::shared_ptr<FPS> makeFPS(float panSpeed, float rotationSpeed);

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        virtual void imgui() override;

    protected:
        virtual FPS* cloneImplementation() override;
        
    private:
        FPS(float panSpeed, float rotationSpeed);
        FPS(const FPS& fps);

        /**
         * The panning speed for the camera.
         */
        float panSpeed;
        /**
         * The rotating speed for the camera.
         */
        float rotationSpeed;
};

namespace pepng {
    std::shared_ptr<FPS> makeFPS(float panSpeed = 0.025f, float rotationSpeed = 0.5f);
}