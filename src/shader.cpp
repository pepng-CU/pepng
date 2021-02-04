#include "shader.hpp"

const std::string readShader(std::filesystem::path filepath) {
    std::ifstream in(filepath);
    
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
        std::cout << ss.str() << std::endl;
        throw std::runtime_error(ss.str());
    }

    return shader;
}

/* Every creation of ShaderBuilder will create a new shader program in opengl.
This is not a problem now but might become one */
ShaderBuilder::ShaderBuilder() {
    shaderProgram = glCreateProgram();
}

ShaderBuilder& ShaderBuilder::operator<<(GLuint shader) {
    shaders.push_back(shader);
    
    return *this;
}

GLuint ShaderBuilder::finish() {
    for (GLuint shader : shaders) {
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
        std::cout << ss.str() << std::endl; // Doesn't render on terminal if not
        throw std::runtime_error(ss.str());
    }

    return shaderProgram;
}