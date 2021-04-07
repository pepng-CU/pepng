#pragma once

#ifdef IMGUI
    #include <imgui.h>
#endif

/**
 * Interface class to print to ImGui.
 */
class WithImGui {
    public:
        virtual void imgui() = 0;
};