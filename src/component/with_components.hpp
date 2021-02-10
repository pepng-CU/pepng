#pragma once

#include <memory>
#include <vector>

#include "../ui/ui.hpp"
#include "component.hpp"

class WithComponents : public WithImGui {
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
            for (auto component : this->components) {
                if(auto ptr = std::dynamic_pointer_cast<T>(component)) {
                    return ptr;
                }
            }

            return nullptr;
        }

        std::vector<std::shared_ptr<Component>> getComponents() {
            return this->components;
        }

    private:
        std::vector<std::shared_ptr<Component>> components;
};