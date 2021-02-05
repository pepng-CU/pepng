#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <filesystem>

namespace utils {
    /**
     * Split a string into an array of words based on a delimiter
     * @param line
     * @param delim
     */
    std::vector<std::string> split(const std::string& line, const std::string& delim = " ");

    /**
     * Uses word split and convert sections into ints.
     * @param line
     * @param delim
     */
    std::vector<int> splitInt(const std::string& line, const std::string& delim = " ");

    /**
     * Searches filesystem to find folder.
     */
    std::filesystem::path getPath(std::filesystem::path relativePath);

    /**
     * Creates and binds a GL buffer.
     */
    template <typename T>
    GLuint bufferFromVector(std::vector<T> vectors, GLenum type) {
        GLuint buffer;

        glGenBuffers(1, &buffer);
        glBindBuffer(type, buffer);
        glBufferData(type, vectors.size() * sizeof(T), &vectors[0], GL_STATIC_DRAW);

        return buffer;
    }

    /**
     * Creates and binds a GL buffer.
     */
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
}