#pragma once

#include "../component.hpp"

/**
 * An FPS like controller for object.
 */
class FPS : public Component {
    public:
        static std::shared_ptr<FPS> make_fps(float panSpeed, float rotationSpeed);

        virtual void update(std::shared_ptr<WithComponents> parent) override;

        #if IMGUI
        virtual void imgui() override;
        #endif

    protected:
        virtual FPS* clone_implementation() override;
        
    private:
        FPS(float panSpeed, float rotationSpeed);
        FPS(const FPS& fps);

        /**
         * The panning speed for the camera.
         */
        float __pan_speed;
        /**
         * The rotating speed for the camera.
         */
        float __rotation_speed;
};

namespace pepng {
    std::shared_ptr<FPS> make_fps(float panSpeed = 0.025f, float rotationSpeed = 0.5f);
}