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
#include <algorithm>

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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /**
     * Debugging message handler.
     */
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
    auto models = Model::fromOBJ("../models/scene.obj");

    /**
     * Matrix creation.
     */
    auto projectionMatrix = glm::perspective(glm::radians(60.0f), 1024.0f / 768.0f, 0.01f, 1000.0f);

    auto worldMatrix = glm::mat4(1.0f);
    worldMatrix = glm::scale(worldMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    worldMatrix = glm::rotate(worldMatrix, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    worldMatrix = glm::translate(worldMatrix, glm::vec3(0.0f, 0.0f, -2.0f));
    

    // Sets background color.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enables Z buffer.
    glEnable(GL_DEPTH_TEST);

    // Culling
    glEnable(GL_CULL_FACE);

    glm::vec3 cameraRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraPosition = glm::vec3(0.0f, 2.0f, 1.0f);

    glfwSetCursorPos(window, 1024.0f / 2.0f, 768.0f / 2.0f);

    // Program loop.
    while (!glfwWindowShouldClose(window)) {
        // Clears depth buffer.
        glClear(GL_DEPTH_BUFFER_BIT);

        // Clears background with buffer color.
        glClear(GL_COLOR_BUFFER_BIT);

        // Activates shader program.
        glUseProgram(shaderProgram);

        double mouseX, mouseY;

        glfwGetCursorPos(window, &mouseX, &mouseY);

        glfwSetCursorPos(window, 1024.0f / 2.0f, 768.0f / 2.0f);

        glm::vec3 rotationDelta = glm::vec3(((float) mouseX - 1024.0f / 2.0f), 0.0f, ((float) mouseY - 768.0f / 2.0f)) / 10.0f;

        cameraRotation += rotationDelta;

        auto rotationMatrix = glm::mat4(1.0f);
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(cameraRotation[2]), glm::vec3(1.0f, 0.0f, 0.0f));
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(cameraRotation[0]), glm::vec3(0.0f, 1.0f, 0.0f));
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(cameraRotation[1]), glm::vec3(0.0f, 0.0f, 1.0f));

        auto viewMatrix = glm::translate(rotationMatrix, -cameraPosition);

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
         * Input.
         */
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPosition -= glm::vec3(rotationMatrix[0][2], 0.0f, rotationMatrix[2][2]) / 10.0f;
        } 
        
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPosition += glm::vec3(rotationMatrix[0][2], 0.0f, rotationMatrix[2][2]) / 10.0f;
        }

        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPosition -= glm::vec3(rotationMatrix[0][0], 0.0f, rotationMatrix[2][0]) / 10.0f;
        }

        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPosition += glm::vec3(rotationMatrix[0][0], 0.0f, rotationMatrix[2][0]) / 10.0f;
        }

        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            cameraPosition.y += 0.1f;
        }

        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            cameraPosition.y -= 0.1f;
        }

        /**
         * GLFW events.
         */
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    glfwTerminate();

    return 0;
}