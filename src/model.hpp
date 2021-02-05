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

#include "utils.hpp"

class Model {
    public:
        Model();

        Model(const Model& model);

        Model(int count, GLuint vao);

        Model(int count, GLuint vao, glm::vec3 pivot);

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

        /**
         * The offset relative to origin. This is used to rotate the offset object.
         * Not sure if this should be here or moved to the object?
         */
        glm::vec3 offset;
};