//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//
// Inspired by the following tutorials:
// - https://learnopengl.com/Getting-started/Hello-Window
// - https://learnopengl.com/Getting-started/Hello-Triangle
#include <iostream>

// This allows linking with Static Library on Windows, without DLL
#define GLEW_STATIC 1 

// Include GLEW - OpenGL Extension Wrangler
#include <GL/glew.h>  

// GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs
#include <GLFW/glfw3.h> 

// GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>

const char *getVertexShaderSource()
{
    // TODO - Insert Vertex Shaders here ...
    // For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
    return "need to return vertex shader source code here...";
}

const char *getFragmentShaderSource()
{
    // TODO - Insert Fragment Shaders here ...
    return "need to return fragment shader source code here...";
}

int compileAndLinkShaders()
{
    // TODO
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    return -1;
}

int createVertexArrayObject()
{
    // TODO
    // Upload geometry to GPU and return the Vertex Buffer Object ID

    return -1;
}

int main(int argc, char *argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

#if defined(PLATFORM_OSX)
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
    GLFWwindow *window = glfwCreateWindow(800, 600, "Comp371 - Lab 01", NULL, NULL);
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
