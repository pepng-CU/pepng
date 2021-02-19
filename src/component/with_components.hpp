#pragma once

#include <memory>
#include <vector>
#include <iostream>

#include "component.hpp"
#include "../ui/with_imgui.hpp"

/**
 * Interface to hold Components.
 */
class WithComponents : public WithImGui, public std::enable_shared_from_this<WithComponents> {
    public:
        /**
         * Attaches a Component to this.
         */
        std::shared_ptr<WithComponents> attachComponent(std::shared_ptr<Component> component);

        /**
         * Updates all the components.
         */
        void updateComponents();

        /**
         * Updates the components that need rendering.
         */
        void renderComponents();

        virtual void imgui();

        virtual std::ostream& operatorOstream(std::ostream& os) const;

        friend std::ostream& operator<<(std::ostream& os, const WithComponents& component);

        /**
         * Generic method to get shared_ptr component of certain type.
         */
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

        /**
         * Generic method to get component of certain type.
         */
        template<typename T>
        std::shared_ptr<T> getComponent() {
            for (auto component : this->components) {
                if(auto ptr = std::dynamic_pointer_cast<T>(component)) {
                    return ptr;
                }
            }

            return nullptr;
        }

        /**
         * Returns all attached components to this.
         */
        std::vector<std::shared_ptr<Component>> getComponents() {
            return this->components;
        }

    private:
        /**
         * Components attached to this.
         */
        std::vector<std::shared_ptr<Component>> components;
};

std::ostream& operator<<(std::ostream& os, const WithComponents& component);