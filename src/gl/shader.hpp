#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstring>
#include <vector>
#include <sstream>

#include <stdarg.h>
#include <GL/glew.h>

namespace pepng {
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

    /**
     * Creates and binds a shader.
     */
    GLuint makeShader(std::filesystem::path filepath, GLenum shaderType);

    /**
     * Creates a shader program from a list of shaders.
     */
    template<typename... Args>
    GLuint makeShaderProgram(Args... shaders) {
        GLuint shaderProgram = glCreateProgram();

        for(auto shader : std::vector<GLuint>{ shaders... }) {
            glAttachShader(shaderProgram, shader);
        }

        glLinkProgram(shaderProgram);

        GLint success;
        char infoLog[512];

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

        if(!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            
            std::stringstream ss;
            ss << "ERROR::PROGRAM::LINKING_FAILED" << std::endl << infoLog << std::endl;

            throw std::runtime_error(ss.str());
        }

        return shaderProgram;
    }
}