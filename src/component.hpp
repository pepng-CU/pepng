#pragma once

#include <memory>
#include <vector>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <string>

#include "io.hpp"
#include "transform.hpp"

class ComponentManager;

class Component {
    public:
        Component(std::string name);

        std::string getName();

        virtual void update(std::shared_ptr<Transform> parent) {};

        virtual void imgui() {};
    private:
        std::string name;
};

class ComponentManager {
    public:
        void attach(std::shared_ptr<Component> component);

        virtual void update() = 0;

        std::vector<std::shared_ptr<Component>> components;
};

class FPSComponent : public Component {
    public:
        FPSComponent(float panSpeed = 0.025f, float rotationSpeed = 0.25f);

        virtual void update(std::shared_ptr<Transform> parent) override;

        virtual void imgui() override;
    private:
        float panSpeed;
        float rotationSpeed;
};

class MovementComponent : public Component {
    public:
        MovementComponent(float positionSpeed = 0.1f, float rotationSpeed = 1.0f);

        virtual void update(std::shared_ptr<Transform> parent) override;

        virtual void imgui() override;
    private:
        float positionSpeed;
        float rotationSpeed;
};

class RenderModeComponent : public Component {
    public:
        RenderModeComponent(std::shared_ptr<GLenum> renderMode);

        virtual void update(std::shared_ptr<Transform> parent) override;

        virtual void imgui() override;
    private:
        std::shared_ptr<GLenum> renderMode;
};