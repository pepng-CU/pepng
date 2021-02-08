#pragma once

#include <memory>

#include "transform.hpp"

class Component {

    private:

    public:
    Component(std::shared_ptr<Transform> obj);
    void keyboardHandler(int);
    void mouseButtonHandler();
    void mouseMovementHandler();
    void mouseScrollWheelHandler();
};