#pragma once

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstring>
#include <vector>
#include <sstream>

/**
 * Reads GLSL file during runtime.
 * 
 * @param path File path to read (relative to runtime).
 */
const std::string readShader(std::filesystem::path filepath);

/**
 * Compiles shader source to shader GL instance.
 * 
 * @param shader Shader source.
 */
GLuint compileShader(const std::string shader, GLenum shaderType);

class ShaderBuilder {
    public:
        ShaderBuilder();
        ShaderBuilder& attach(GLuint shader);
        GLuint build();

    private:
        std::vector<GLuint> shaders;
        GLuint shaderProgram;
};