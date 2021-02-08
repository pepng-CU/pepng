#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "transform.hpp"

class Component {
    public:
        Component() {}

        virtual void update() = 0;
        virtual void mouseButtonCallback(int button, int action) = 0;
        virtual void keyboardCallback(int key, int action) = 0;
        virtual void scrollCallback(glm::vec2 delta) = 0;
        virtual void cursorPositionCallback(glm::vec2 delta) = 0;
};

class FPSComponent : public Component {
    public:
        FPSComponent(std::shared_ptr<Transform> object);

        virtual void update() override;
        virtual void mouseButtonCallback(int button, int action) override;
        virtual void keyboardCallback(int key, int action) override;
        virtual void scrollCallback(glm::vec2 delta) override;
        virtual void cursorPositionCallback(glm::vec2 delta) override;
    
    private:
        bool isPanning;
        bool isRotating;

        std::shared_ptr<Transform> object;
};