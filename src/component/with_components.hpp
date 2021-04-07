#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <sstream>

#include "component.hpp"
#include "../ui/with_imgui.hpp"

/**
 * Interface to hold Components.
 */
class WithComponents : 
    #ifdef IMGUI
    public WithImGui, 
    #endif
    public std::enable_shared_from_this<WithComponents> 
{
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

        virtual std::ostream& operator_ostream(std::ostream& os) const;

        friend std::ostream& operator<<(std::ostream& os, const WithComponents& component);

        #ifdef IMGUI
        virtual void imgui();
        #endif

        /**
         * Get shared_ptr components of certain type.
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
         * 
         * @throw If component cannot be found.
         */
        template<typename T>
        std::shared_ptr<T> try_get_component() {
            auto component = this->get_component<T>();

            if (!component) { 
                std::stringstream ss;

                ss << *this << " could not find a component." << std::endl;

                std::cout << ss.str() << std::endl;

                throw std::runtime_error(ss.str());
            }

            return component;
        }

        /**
         * Get component of certain type.
         * 
         * @return nullptr if component cannot be found.
         */
        template<typename T>
        std::shared_ptr<T> get_component() {
            for (auto component : this->components) {
                if(auto ptr = std::dynamic_pointer_cast<T>(component)) {
                    return ptr;
                }
            }

            return std::shared_ptr<T>(nullptr);
        }

        /**
         * Checks if component is attached.
         */
        template<typename T>
        bool has_component() {
            if(this->get_component<T>()) return true;

            return false;
        }

        /**
         * Replaces all components of certain type with new component.
         */
        template<typename T>
        void replace_components(std::shared_ptr<T> component) {
            for(int i = 0; i < this->components.size(); i++) {
                if(std::dynamic_pointer_cast<T>(this->components.at(i))) {
                    this->components.at(i) = component;
                }
            }
        }

        /**
         * Removes all components of certain type with new component.
         */
        template<typename T>
        void remove_components() {
            std::vector<std::shared_ptr<Component>> new_components;

            for(int i = 0; i < this->components.size(); i++) {
                if(!std::dynamic_pointer_cast<T>(this->components.at(i))) {
                    new_components.push_back(this->components.at(i));
                }
            }

            this->components = new_components;
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