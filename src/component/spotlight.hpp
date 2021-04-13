/**
 * This is the standard way for defining new Component hpp for PEPNG.
 * 
 * Every hpp should only contain one Component definition.
 */
#pragma once

#include "light.hpp"

/**
 * Component for Spotlight.
 */
class Spotlight : public Light {
    public:
        /**
         * OO METHODS
         */

        /**
         * Shared_ptr constructor for Spotlight.
         * 
         * This method is used over standard constructors as PEPNG standards.
         */
        static std::shared_ptr<Spotlight> make_spotlight(GLuint shader_program, float angle, glm::vec3 color, float intensity);

        /**
         * LIFETIME METHODS
         */

        virtual void delayed_init() override;

        // Attaches Light to shader program.
        virtual void render(GLuint shader_program) override;

        // Initializes the Frame buffer.
        virtual void init_fbo() override;

        // This light projection matrix.
        glm::mat4 matrix();

        /**
         * IMGUI
         * 
         * Given IMGUI is optional, you need to check if it is present.
         */
        #ifdef IMGUI
        virtual void imgui() override;
        #endif
    protected:
        /**
         * CLONE IMPL
         * 
         * We need to supply a raw clone for the shared_ptr.
         */
        virtual Spotlight* clone_implementation() override;
    
    private:
        /**
         * CONSTRUCTORS
         * 
         * Constructors are made intentionally private to force usage of shared_ptr constructor.
         */

        // Base constructor.
        Spotlight(GLuint shader_program, float angle, glm::vec3 color, float intensity);
        // Copy constructor. MUST be implemented for all components.
        Spotlight(const Spotlight& spotlight);

        /**
         * MEMBER VARIABLES
         */

        // The spotlight angle in degrees.
        float __angle;

        int __index;
        static int __count;
};

// Gives access to shared_ptr constructor to the pepng namespace.
namespace pepng {
    /**
     * Shader_ptr constructor for Spotlight.
     * 
     * Uses the inner Spotlight contructor.
     */
    std::shared_ptr<Spotlight> make_spotlight(GLuint shader_program, float angle, glm::vec3 color, float intensity);
}