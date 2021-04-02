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
        std::shared_ptr<WithComponents> attach_component(std::shared_ptr<Component> component);

        /**
         * Updates all the components.
         */
        void update_components();

        /**
         * Updates the components that need rendering.
         */
        void render_components();

        virtual void imgui();

        virtual std::ostream& operator_ostream(std::ostream& os) const;

        friend std::ostream& operator<<(std::ostream& os, const WithComponents& component);

        /**
         * Generic method to get shared_ptr component of certain type.
         */
        template<typename T>
        std::vector<std::shared_ptr<T>> get_components() {
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
        std::shared_ptr<T> get_component() {
            for (auto component : this->components) {
                if(auto ptr = std::dynamic_pointer_cast<T>(component)) {
                    return ptr;
                }
            }

            throw std::runtime_error("Could not find Component.");
        }

        /**
         * Returns all attached components to this.
         */
        std::vector<std::shared_ptr<Component>> get_components() {
            return this->components;
        }

    protected:
        WithComponents();
        WithComponents(const WithComponents& withComponents);

    private:
        /**
         * Components attached to this.
         */
        std::vector<std::shared_ptr<Component>> components;
};

std::ostream& operator<<(std::ostream& os, const WithComponents& component);