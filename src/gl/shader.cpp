#include "shader.hpp"

const std::string pepng::readShader(std::filesystem::path filepath) {
    std::ifstream in(filepath);
    
    std::string contents((std::istreambuf_iterator<char>(in)), 
        std::istreambuf_iterator<char>());

    return contents;
}

GLuint pepng::compileShader(const std::string shaderSource, GLenum shaderType) {
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

GLuint pepng::makeShader(std::filesystem::path filepath, GLenum shaderType) {
    return pepng::compileShader(pepng::readShader(filepath), shaderType);
}