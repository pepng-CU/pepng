#pragma once

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstring>

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
 * Compiles vertex + fragment shader to GL program.
 * 
 * @param vertex Vertex shader.
 * @param fragment Fragment shader.
 */
GLuint linkProgram(GLuint vertex, GLuint fragment);