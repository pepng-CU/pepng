#include "shader.hpp"

const std::string readShader(const char* path) {
    std::filesystem::path file(path);
    std::filesystem::path fullPath = std::filesystem::current_path() / file;

    std::ifstream in(fullPath);
    
    std::string contents((std::istreambuf_iterator<char>(in)), 
        std::istreambuf_iterator<char>());

    return contents;
}

GLuint compileShader(const std::string shaderSource, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);

    const char* shaderSourceCStr = shaderSource.c_str();
    glShaderSource(shader, 1, &shaderSourceCStr, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        
        std::stringstream ss;

        ss << "ERROR::SHADER::COMPILATION_FAILED" << std::endl << infoLog << std::endl;

        throw std::runtime_error(ss.str());
    }

    return shader;
}

GLuint linkProgram(GLuint vertex, GLuint fragment) {
    GLuint program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    int success;
    char infoLog[512];

    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);

        std::stringstream ss;

        ss << "ERROR::PROGRAM::LINKING_FAILED" << std::endl << infoLog << std::endl;

        throw std::runtime_error(ss.str());
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}