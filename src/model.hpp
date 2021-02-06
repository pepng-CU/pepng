#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h> 
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <stb_image.h>

#include "utils.hpp"

class Model {
    public:
        Model();

        Model(const Model& model);

        /**
         * Renders model using shader program.
         * @param program Shader program to use.
         */
        void render(GLuint program, GLenum mode = GL_TRIANGLES);

        /**
         * Returns the translation performed by the pivot.
         * Will be used in object - so not sure if it should just be moved there?
         */
        glm::mat4 getOffsetMatrix();

        glm::mat4 getNegativeOffsetMatrix();

        /**
         * Reads models from OBJ file.
         * @param program Relative path to OBJ file.
         */
        static std::vector<std::shared_ptr<Model>> fromOBJ(std::filesystem::path filepath);
    // private:
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

class ModelBuilder {
    public:
        ModelBuilder();

        std::shared_ptr<Model> build();

        ModelBuilder* setName(std::string name);

        ModelBuilder* setCount(unsigned int count);

        ModelBuilder* calculateOffset(std::vector<glm::vec3> vertexArray, std::vector<unsigned int> faceArray);
        
        /**
         * Creates and binds a GL buffer.
         */
        template <typename T>
        ModelBuilder* attachBuffer(std::vector<T> vectors, GLenum type, bool isCount = false) {
            GLuint buffer;

            glGenBuffers(1, &buffer);
            glBindBuffer(type, buffer);
            glBufferData(type, vectors.size() * sizeof(T), &vectors[0], GL_STATIC_DRAW);

            if(isCount) {
                this->model->count = vectors.size();
            }

            // TODO: better check for this...
            this->model->hasElementArray = true;

            return this;
        }

        /**
         * Creates and binds a GL buffer.
         */
        template <typename T>
        ModelBuilder* attachBuffer(std::vector<T> vectors, GLenum type, int index, int size) {
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

            return this;
        }
    private:
        std::shared_ptr<Model> model;
};

/**
 * Creates and binds texture.
 * 
 * TODO: definitely clean this up!
 */
GLuint createTexture(std::filesystem::path filePath);