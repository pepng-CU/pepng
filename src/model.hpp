#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <memory>
#include <sstream>

#include "utils.hpp"

template <typename T>
GLuint bufferFromVector(std::vector<T> vectors, GLenum type) {
    GLuint buffer;

    glGenBuffers(1, &buffer);
    glBindBuffer(type, buffer);
    glBufferData(type, vectors.size() * sizeof(T), &vectors[0], GL_STATIC_DRAW);

    return buffer;
}

template <typename T>
GLuint bufferFromVector(std::vector<T> vectors, GLenum type, int index) {
    GLuint buffer;

    glGenBuffers(1, &buffer);
    glBindBuffer(type, buffer);
    glBufferData(type, vectors.size() * sizeof(T), &vectors[0], GL_STATIC_DRAW);

    glVertexAttribPointer(
        index, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        0, 
        0
    );

    glEnableVertexAttribArray(index);

    return buffer;
}

class Model {
    public:
        Model(int count, GLuint vao);

        /**
         * Renders model using shader program.
         * @param program Shader program to use.
         */
        void render(GLuint program, GLenum mode = GL_TRIANGLES);

        /**
         * Reads models from OBJ file.
         * @param program Relative path to OBJ file.
         */
        static std::vector<std::shared_ptr<Model>> fromOBJ(std::filesystem::path filepath);

        /**
         * Model from vector params.
         */
        static std::shared_ptr<Model> fromVectors(
            std::vector<glm::vec3> vertexArray, 
            std::vector<glm::vec2> textureArray, 
            std::vector<unsigned int> faceArray
        );
    //private:
        // Keeps count of the number of faces.
        int count;

        // Vertex array object that is used for drawing this model
        GLuint vao;
};