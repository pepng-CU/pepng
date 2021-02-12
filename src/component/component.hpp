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

#include "../ui/with_imgui.hpp"

class WithComponents;

class Component : public WithImGui {
    public:
        bool isActive;

        Component(std::string name);

        std::string getName();

        virtual std::ostream& operatorOstream(std::ostream& os) const;

        friend std::ostream& operator<<(std::ostream& os, const Component& component);

        virtual void init(std::shared_ptr<WithComponents> parent) {};

        virtual void render(std::shared_ptr<WithComponents> parent) {};

        virtual void update(std::shared_ptr<WithComponents> parent) {};

        virtual void imgui() override;

    private:
        std::string name;
};

std::ostream& operator<<(std::ostream& os, const Component& component);

#include "with_components.hpp"