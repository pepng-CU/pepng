#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>

#include "transform.hpp"

class Component {
    public:
        Component() {}

        virtual void update() {};
        virtual void mouseButtonCallback(GLFWwindow* window, int button, int action) {};
        virtual void keyboardCallback(GLFWwindow* window, int key, int action) {};
        virtual void scrollCallback(GLFWwindow* window, glm::vec2 delta) {};
        virtual void cursorPositionCallback(GLFWwindow* window, glm::vec2 delta) {};
};

class FPSComponent : public Component {
    public:
        FPSComponent(std::shared_ptr<Transform> object, float panSpeed = 0.025f, float rotationSpeed = 0.25f);

        virtual void mouseButtonCallback(GLFWwindow* window, int button, int action) override;
        virtual void scrollCallback(GLFWwindow* window, glm::vec2 delta) override;
        virtual void cursorPositionCallback(GLFWwindow* window, glm::vec2 delta) override;
    private:
        float panSpeed;
        float rotationSpeed;

        bool isPanning;
        bool isRotating;

        std::shared_ptr<Transform> object;
};

class MovementComponent : public Component {
    public:
        MovementComponent(std::shared_ptr<Transform> object, float positionSpeed = 0.1f, float rotationSpeed = 1.0f);

        glm::vec3 getDeltaPosition(int key);
        glm::vec3 getDeltaRotation(int key);

        virtual void update() override;
        virtual void keyboardCallback(GLFWwindow* window, int key, int action) override;
    private:
        float positionSpeed;
        float rotationSpeed;

        glm::vec3 deltaPosition;
        glm::vec3 deltaRotation;
        std::shared_ptr<Transform> object;
};

class ObjectManagerComponent : public Component {
    public:
        ObjectManagerComponent(std::vector<std::shared_ptr<Component>> components);

        virtual void update() override;
        virtual void keyboardCallback(GLFWwindow* window, int key, int action) override;
    private:
        int objectIndex;
        std::vector<std::shared_ptr<Component>> components;
};

class EscapeComponent : public Component {
    public:
        virtual void keyboardCallback(GLFWwindow* window, int key, int action) override;
};

class RenderModeComponent : public Component {
    public:
        RenderModeComponent(std::shared_ptr<GLenum> renderMode);

        GLenum getRenderMode(int key);
        bool hasRenderMode(int key);

        virtual void keyboardCallback(GLFWwindow* window, int key, int action) override;
    private:
        std::shared_ptr<GLenum> renderMode;
};