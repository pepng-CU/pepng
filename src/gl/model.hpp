#pragma once

#include <unordered_set>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <memory>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Renderer;

#include "buffer.hpp"
#include "texture.hpp"
#include "../util/delayed_init.hpp"
#include "../util/utils.hpp"

class Renderer;

/**
 * The geometry for a Renderer.
 * 
 * This is essentially just a VAO constructor that hold the points.
 */
class Model : public std::enable_shared_from_this<Model>, public DelayedInit, public Cloneable2<Model>  {
    public:
        /**
         * Shared_ptr constructor for Model.
         */
        static std::shared_ptr<Model> make_model();

        /**
         * Accessor for name.
         */
        inline std::string name() { return this->__name; }

        /**
         * Accessor for count.
         */
        inline unsigned int count() { return this->__count; }

        /**
         * Accessor for VAO.
         */
        inline GLuint vao() { return this->__vao; }

        /**
         * Accessor for offset.
         */
        glm::vec3 offset() { return this->__offset; }

        /**
         * Accessor for has element array.
         */
        bool has_element_array() { return this->__has_element_array; }

        /**
         * Mutator for name.
         */
        std::shared_ptr<Model> set_name(std::string name) {     
            this->__name = name;

            return shared_from_this(); 
        }

        /**
         * Mutator for the number of indices (if the model uses indices).
         */
        inline std::shared_ptr<Model> set_count(unsigned int count) {     
            this->__count = count;

            return shared_from_this(); 
        }

        /**
         * Mutator that sets if this uses an element array.
         */
        std::shared_ptr<Model> set_element_array(bool hasElementArray) {     
            this->__has_element_array = hasElementArray;

            return shared_from_this(); 
        }

        /**
         * Calculates the geometry average position as an offset to the object.
         */
        std::shared_ptr<Model> calculate_offset(std::vector<glm::vec3> vertexArray, std::vector<unsigned int> faceArray);

        virtual void delayed_init() override;

        /**
         * Binds buffer.
         */
        template <typename T>
        std::shared_ptr<Model> attach_buffer(std::shared_ptr<Buffer<T>> buffer) {
            this->attach_delayed(buffer);

            return shared_from_this();
        }

    protected:
        virtual Model* clone_implementation() override;

    private:
        Model();
        Model(const Model& model);

        // Keeps count of the number of faces.
        unsigned int __count;

        // Vertex array object that is used for drawing this model
        GLuint __vao;

        /**
         * The offset relative to origin. This is used to rotate the offset object.
         * Not sure if this should be here or moved to the object?
         */
        glm::vec3 __offset;

        /**
         * The model name.
         */
        std::string __name;

        /**
         * Variable to check if using element array.
         */
        bool __has_element_array;
};

namespace pepng {
    std::shared_ptr<Model> make_model();
}

#include "../component/renderer.hpp"