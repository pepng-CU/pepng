#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>

#include "io.hpp"
#include "transform.hpp"

class ComponentManager;

class Component {
    public:
        Component() {}

        virtual void update(std::shared_ptr<Transform> parent) {};
};

class ComponentManager {
    public:
        void attach(std::shared_ptr<Component> component);

        virtual void update() = 0;

    protected:
        std::vector<std::shared_ptr<Component>> components;
};

class FPSComponent : public Component {
    public:
        FPSComponent(float panSpeed = 0.025f, float rotationSpeed = 0.25f);

        virtual void update(std::shared_ptr<Transform> parent) override;
    private:
        float panSpeed;
        float rotationSpeed;
};

class MovementComponent : public Component {
    public:
        MovementComponent(float positionSpeed = 0.1f, float rotationSpeed = 1.0f);

        virtual void update(std::shared_ptr<Transform> parent) override;
    private:
        float positionSpeed;
        float rotationSpeed;
};

class RenderModeComponent : public Component {
    public:
        RenderModeComponent(std::shared_ptr<GLenum> renderMode);

        virtual void update(std::shared_ptr<Transform> parent) override;
    private:
        std::shared_ptr<GLenum> renderMode;
};