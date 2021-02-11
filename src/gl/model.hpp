#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <memory>
#include <sstream>
#include <string>

class Renderer;

#include "buffer.hpp"
#include "texture.hpp"
#include "../util/delayed_init.hpp"
#include "../util/utils.hpp"

class Renderer;

class Model : public std::enable_shared_from_this<Model>, public DelayedInit  {
    public:
        friend Renderer;

        static std::shared_ptr<Model> makeModel();

        std::shared_ptr<Model> setName(std::string name);

        std::string getName();

        std::shared_ptr<Model> setCount(unsigned int count);

        std::shared_ptr<Model> setElementArray(bool hasElementArray);

        std::shared_ptr<Model> calculateOffset(std::vector<glm::vec3> vertexArray, std::vector<unsigned int> faceArray);

        virtual void delayedInit() override;

        /**
         * Binds buffer.
         */
        template <typename T>
        std::shared_ptr<Model> attach(std::shared_ptr<Buffer<T>> buffer) {
            this->attachDelayed(buffer);

            return shared_from_this();
        }

    private:
        Model();

        // Keeps count of the number of faces.
        int count;

        // Vertex array object that is used for drawing this model
        GLuint vao;

        /**
         * The offset relative to origin. This is used to rotate the offset object.
         * Not sure if this should be here or moved to the object?
         */
        glm::vec3 offset;

        /**
         * The model name.
         */
        std::string name;

        /**
         * Variable to check if using element array.
         */
        bool hasElementArray;
};

namespace pepng {
    std::shared_ptr<Model> makeModel();
}

#include "../component/renderer.hpp"