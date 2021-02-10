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

#include "../util/utils.hpp"
#include "../gl/texture.hpp"

class Renderer;

class Model : public std::enable_shared_from_this<Model>  {
    public:
        friend Renderer;

        static std::shared_ptr<Model> makeModel();

        std::shared_ptr<Model> setName(std::string name);

        std::string getName();

        std::shared_ptr<Model> setCount(unsigned int count);

        std::shared_ptr<Model> calculateOffset(std::vector<glm::vec3> vertexArray, std::vector<unsigned int> faceArray);

        /**
         * Reads models from OBJ file.
         * @param program Relative path to OBJ file.
         */
        static std::vector<std::shared_ptr<Model>> fromOBJ(std::filesystem::path filepath);

        /**
         * Creates and binds a GL buffer.
         */
        template <typename T>
        std::shared_ptr<Model> attachBuffer(std::vector<T> vectors, GLenum type, bool isCount = false) {
            GLuint buffer;

            glGenBuffers(1, &buffer);
            glBindBuffer(type, buffer);
            glBufferData(type, vectors.size() * sizeof(T), &vectors[0], GL_STATIC_DRAW);

            if(isCount) {
                this->count = vectors.size();
            }

            // TODO: better check for this...
            this->hasElementArray = true;

            return shared_from_this();
        }

        /**
         * Creates and binds a GL buffer.
         */
        template <typename T>
        std::shared_ptr<Model> attachBuffer(std::vector<T> vectors, GLenum type, int index, int size) {
            GLuint buffer;

            glGenBuffers(1, &buffer);
            glBindBuffer(type, buffer);
            glBufferData(type, vectors.size() * sizeof(T), &vectors[0], GL_STATIC_DRAW);

            glVertexAttribPointer(
                index, 
                size, 
                GL_FLOAT, 
                GL_FALSE, 
                0, 
                0
            );

            glEnableVertexAttribArray(index);

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

#include "../component/renderer.hpp"