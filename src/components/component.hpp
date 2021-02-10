#pragma once

#include <memory>
#include <vector>
#include <imgui.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <string>

#include "../io/io.hpp"
#include "../components/transform.hpp"

class ComponentManager;

class Component {
    public:
        bool isActive;

        Component(std::string name);

        std::string getName();

        virtual void update(std::shared_ptr<Transform> parent) {};

        virtual void imgui();

    private:
        std::string name;
};

class ComponentManager {
    public:
        void attach(std::shared_ptr<Component> component);

        virtual void update() = 0;

        virtual void imgui();

        template<typename T>
        std::vector<std::shared_ptr<T>> getComponents() {
            std::vector<std::shared_ptr<T>> components;

            for (auto component : this->components) {
                if(auto ptr = std::dynamic_pointer_cast<T>(component)) {
                    components.push_back(ptr);
                }
            }

            return components;
        }

        template<typename T>
        std::shared_ptr<T> getComponent() {
            return this->getComponents<T>().at(0);
        }

        std::vector<std::shared_ptr<Component>> getComponents() {
            return this->components;
        }

    private:
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