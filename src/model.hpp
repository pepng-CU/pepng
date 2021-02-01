#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <memory>

#include "utils.hpp"

class Model {
    public:
        Model(int count, GLuint vertexBuffer, GLuint faceBuffer);

        /**
         * Renders model using shader program.
         * @param program Shader program to use.
         */
        void render(GLuint program);

        /**
         * Reads models from OBJ file.
         * @param program Relative path to OBJ file.
         */
        static std::vector<std::shared_ptr<Model>> fromOBJ(const char* path);

        /**
         * Model from vector params.
         */
        static std::shared_ptr<Model> fromVectors(std::vector<glm::vec3> vertexArray, std::vector<unsigned int> faceArray);
    //private:
        // Keeps count of the number of faces.
        int count;

        // Buffer reference for verticies.
        GLuint vertexBuffer;
        
        // Buffer reference for faces.
        GLuint faceBuffer;
};