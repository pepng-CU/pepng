/**
 * COMP371 Final Project
 * 
 * Thank you for the template creator: Nicolas Bergeron on 20/06/2019
 * 
 * @author Alexandre Lavoie (40129457), Antoine Poulin (#), Rhys Rosenberg (#), Arman (#)
 */

#define GLEW_STATIC 1

// External includes
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

// Internal includes
#include "shader.hpp"
#include "model.hpp"

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam ) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
                type, severity, message );
}

std::vector<glm::vec3> CUBE_VERTICIES {
    // Front face
    glm::vec3(-1.0, -1.0, 1.0),
    glm::vec3(1.0, -1.0, 1.0),
    glm::vec3(1.0, 1.0, 1.0),
    glm::vec3(-1.0, 1.0, 1.0),
    // Back face
    glm::vec3(-1.0, -1.0, -1.0),
    glm::vec3(-1.0, 1.0, -1.0),
    glm::vec3(1.0, 1.0, -1.0),
    glm::vec3(1.0, -1.0, -1.0),
    // Top face
    glm::vec3(-1.0, 1.0, -1.0),
    glm::vec3(-1.0, 1.0, 1.0),
    glm::vec3(1.0, 1.0, 1.0),
    glm::vec3(1.0, 1.0, -1.0),
    // Bottom face
    glm::vec3(-1.0, -1.0, -1.0),
    glm::vec3(1.0, -1.0, -1.0),
    glm::vec3(1.0, -1.0, 1.0),
    glm::vec3(-1.0, -1.0, 1.0),
    // Right face
    glm::vec3(1.0, -1.0, -1.0),
    glm::vec3(1.0, 1.0, -1.0),
    glm::vec3(1.0, 1.0, 1.0),
    glm::vec3(1.0, -1.0, 1.0),
    // Left face
    glm::vec3(-1.0, -1.0, -1.0),
    glm::vec3(-1.0, -1.0, 1.0),
    glm::vec3(-1.0, 1.0, 1.0),
    glm::vec3(-1.0, 1.0, -1.0)
};

std::vector<unsigned int> CUBE_FACES {
    // Front face
    0, 1, 2, 0, 2, 3,
    // Back face
    4, 5, 6, 4, 6, 7,
    // Top face
    8, 9, 10, 8, 10, 11,
    // Bottom face
    12, 13, 14, 12, 14, 15,
    // Right face
    16, 17, 18, 16, 18, 19,
    // Left face
    20, 21, 22, 20, 22, 23
};

int main(int argc, char *argv[]) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(1024, 768, "Comp371 - Final Project", NULL, NULL);

    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    /**
     * Shader binding
     */
    std::string vertexSource = readShader("../shaders/vertex.glsl");

    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);

    std::string fragmentSource = readShader("../shaders/fragment.glsl");

    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    GLuint shaderProgram = linkProgram(vertexShader, fragmentShader);

    /**
     * VAO enable
     */
    GLuint VAO;

    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    /**
     * Model creation.
     */
    // Render basic cube with inline vectors.
    // auto models = std::vector<std::shared_ptr<Model>>{Model::fromVectors(CUBE_VERTICIES, CUBE_FACES)};

    // Gets objects from obj file.
    auto models = Model::fromOBJ("../models/alex.obj");

    /**
     * Matrix creation.
     */
    auto projectionMatrix = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.01f, 1000.0f);

    auto viewMatrix = 
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    auto worldMatrix = 
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f)) *
        glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    // Sets background color.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enables Z buffer.
    glEnable(GL_DEPTH_TEST);

    float rotation = 0.0f;

    // Program loop.
    while (!glfwWindowShouldClose(window)) {
        // Clears depth buffer.
        glClear(GL_DEPTH_BUFFER_BIT);

        // Clears background with buffer color.
        glClear(GL_COLOR_BUFFER_BIT);

        // Activates shader program.
        glUseProgram(shaderProgram);

        // Sets object rotation.
        worldMatrix = 
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f)) *
            glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(1.0f, 1.0f, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

        rotation += 0.01f;

        /**
         * Uniform matrix binding.
         */
        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "u_projection"),
            1,
            GL_FALSE,
            glm::value_ptr(projectionMatrix)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "u_view"),
            1,
            GL_FALSE,
            glm::value_ptr(viewMatrix)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "u_world"),
            1,
            GL_FALSE,
            glm::value_ptr(worldMatrix)
        );

        // Renders all models from OBJ file.
        for(auto model : models) {
            model->render(shaderProgram);
        }

        /**
         * GLFW events.
         */
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    glfwTerminate();

    return 0;
}