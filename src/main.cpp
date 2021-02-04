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
#include <filesystem>

// Internal includes
#include "shader.hpp"
#include "model.hpp"
#include "utils.hpp"
#include "camera.hpp"
#include "transform.hpp"

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam ) {
    // fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    //         ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
    //             type, severity, message );
}

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

    // Binds to refresh rate.
    glfwSwapInterval(1);

    /**
     * Debugging message handler.
     */
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    // Find the shaders directory
    auto shaderpath = utils::getPath("shaders");

    /**
     * Shader binding
     */
    std::string vertexSource = readShader(shaderpath / "vertex.glsl");
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);

    std::string fragmentSource = readShader(shaderpath / "fragment.glsl");
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    ShaderBuilder sb;

    sb << vertexShader << fragmentShader;

    GLuint shaderProgram = sb.finish();

    /**
     * Model creation.
     */
    // Find model path
    auto modelpath = utils::getPath("models");

    // Gets objects from obj file.
    auto models = Model::fromOBJ(modelpath / "scene.obj");
    
    // Sets background color.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enables Z buffer.
    glEnable(GL_DEPTH_TEST);

    // Culling
    glEnable(GL_CULL_FACE);

    Transform objectTransform = Transform {
        glm::vec3(0.0f, 0.0f, -2.0f),
        glm::vec3(0.0f),
        glm::vec3(1.0f)
    };

    Camera camera = Camera {
        Transform {
            glm::vec3(0.0f, 2.0f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(1.0f)
        },
        Viewport {
            glm::vec2(0.0f),
            glm::vec2(1024.0f, 768.0f)
        },
        glm::perspective(glm::radians(60.0f), 1024.0f / 768.0f, 0.01f, 1000.0f)
    };

    glm::vec2 clickPosition = glm::vec2();

    // Program loop.
    while (!glfwWindowShouldClose(window)) {
        // Clears depth + color buffer.
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // Activates shader program.
        glUseProgram(shaderProgram);

        /**
         * Uniform matrix binding.
         */
        camera.render(shaderProgram);

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "u_world"),
            1,
            GL_FALSE,
            glm::value_ptr(objectTransform.getWorldMatrix())
        );

        // Renders all models from OBJ file.
        for(auto model : models) {
            model->render(shaderProgram);
        }

        /**
         * Input.
         */
        double mouseX, mouseY;

        glfwGetCursorPos(window, &mouseX, &mouseY);

        bool leftClicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        bool middleClicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
        bool rightClicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

        glm::vec3 cameraForward = camera.transform.getForward();
        glm::vec3 cameraRight = camera.transform.getRight();
        glm::vec3 cameraUp = camera.transform.getUp();

        if (leftClicked || rightClicked || middleClicked) {
            if (clickPosition.x == 0 && clickPosition.y == 0) {
                clickPosition = glm::vec2((float) mouseX, (float) mouseY);
            }
        } else {
            clickPosition = glm::vec2();
        }

        float mouseDx = std::clamp(((float) mouseX - clickPosition.x) / 40.0f, -3.0f, 3.0f);
        float mouseDy = std::clamp(((float) mouseY - clickPosition.y) / 40.0f, -3.0f, 3.0f);

        if (leftClicked) {
            camera.transform.position -= cameraForward * mouseDy / 10.0f;
        }

        if (middleClicked) {
            camera.transform.position += cameraUp * mouseDy / 10.0f + cameraRight * mouseDx / 10.0f;
        }

        if (rightClicked) {
            camera.transform.deltaRotate(mouseDx, mouseDy);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.transform.position += cameraForward / 10.0f;
        } 
        
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.transform.position -= cameraForward / 10.0f;
        }

        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.transform.position -= cameraRight / 10.0f;
        }

        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.transform.position += cameraRight / 10.0f;
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