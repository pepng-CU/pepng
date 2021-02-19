#pragma once

#include <imgui.h>

/**
 * Interface class to print to ImGui.
 */
class WithImGui {
    public:
        virtual void imgui() = 0;
};