#pragma once

#include <memory>
#include <vector>

#include "component.hpp"
#include "../ui/with_imgui.hpp"

class WithComponents : public WithImGui, public std::enable_shared_from_this<WithComponents> {
    public:
        std::shared_ptr<WithComponents> attachComponent(std::shared_ptr<Component> component);

        void updateComponents();

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