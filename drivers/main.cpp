#include <iostream>

#define GLEW_STATIC 1 

#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>

const char *getVertexShaderSource() {
    // TODO: Load vert shader.
    return "need to return vertex shader source code here...";
}

const char *getFragmentShaderSource() {
    // TODO: Load frag shader.
    return "need to return fragment shader source code here...";
}

int compileAndLinkShaders() {
    // TODO: Compile + link shaders.
    return -1;
}

int createVertexArrayObject() {
    // TODO: Upload geometry to GPU and return the Vertex Buffer Object ID
    return -1;
}

int main(int argc, char *argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

#if defined(__unix__)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#elif defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // On windows, we set OpenGL version to 2.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow *window = glfwCreateWindow(800, 600, "Comp371 - Project", NULL, NULL);
    
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Compile and link shaders here ...
    int shaderProgram = compileAndLinkShaders();

    // Define and upload geometry to the GPU here ...
    int vao = createVertexArrayObject();

    // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO - draw rainbow triangle

        // End frame
        glfwSwapBuffers(window);

        // Detect inputs
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    // Shutdown GLFW
    glfwTerminate();

    return 0;
}
