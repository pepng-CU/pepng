/**
 * COMP371 Final Project
 * 
 * Thank you for the template creator: Nicolas Bergeron on 20/06/2019
 * 
 * @author Alexandre Lavoie (40129457), Antoine Poulin (#), Rhys Rosenberg (#), Arman (#)
 */

#define GLEW_STATIC 1
#define STB_IMAGE_IMPLEMENTATION

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
#include <stdlib.h>
#include <time.h>

// Internal includes
#include "shader.hpp"
#include "model.hpp"
#include "utils.hpp"
#include "camera.hpp"
#include "transform.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "controller.hpp"
#include "component.hpp"

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
    srand(time(NULL));

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

    /**
     * Texture loading
     */
    // Find the textures directory
    auto texturespath = utils::getPath("textures");

    #ifdef _MSC_VER
        createTexture((const std::string&)(texturespath / "honeycomb.jpg").u8string());
    #else
        createTexture((const std::string&)(texturespath / "honeycomb.jpg"));
    #endif

    /**
     * Shader loading
     */
    // Find the shaders directory
    auto shaderpath = utils::getPath("shaders");

    // Object shader
    std::string vertexSource = readShader(shaderpath / "object/vertex.glsl");
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);

    std::string fragmentSource = readShader(shaderpath / "object/fragment.glsl");
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    ShaderBuilder sb;

    sb << vertexShader << fragmentShader;

    GLuint shaderProgram = sb.finish();

    // Line shader
    vertexSource = readShader(shaderpath / "line/vertex.glsl");
    vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);

    fragmentSource = readShader(shaderpath / "line/fragment.glsl");
    fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    sb = ShaderBuilder();

    sb << vertexShader << fragmentShader;

    GLuint lineShaderProgram = sb.finish();

    /**
     * Models
     */
    // Find model path
    auto modelpath = utils::getPath("models");

    std::vector<std::shared_ptr<Object>> objects {
        Object::fromOBJ(modelpath / "scene.obj", shaderProgram, Transform {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        })
    };

    /**
     * Cameras
     */
    std::vector<std::shared_ptr<Camera>> cameras {
        std::make_shared<Camera>(
            Transform {
                glm::vec3(0.0f, 2.0f, 10.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 1.0f, 1.0f)
            },
            Viewport {
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1024.0f, 768.0f)
            },
            glm::perspective(glm::radians(60.0f), 1024.0f / 768.0f, 0.01f, 1000.0f)
        )
    };

    /**
     * Lines
     */
    std::vector<std::shared_ptr<Object>> lines {
        std::static_pointer_cast<Object>(
            std::make_shared<Axes>(Transform {
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(7.0f, 7.0f, 7.0f)
            }, lineShaderProgram)
        ),
        std::static_pointer_cast<Object>(
            std::make_shared<Grid>(Transform {
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(128.0f, 128.0f, 128.0f)
            }, lineShaderProgram, 129)
        )
    };

    /**
     * Controller
     */
    auto controller = new Controller(window);

    Controller::setInstance(controller);

    controller->attach(std::make_shared<FPSComponent>(std::static_pointer_cast<Transform>(cameras.at(0))));

    std::vector<std::shared_ptr<Component>> objectComponents;

    // TODO: Recursive object selection.
    for(auto object : objects) {
        objectComponents.push_back(
            std::static_pointer_cast<Component>(
                std::make_shared<MovementComponent>(
                    object
                )
            )
        );
    }

    controller->attach(std::make_shared<ObjectManagerComponent>(objectComponents)); 

    /**
     * OpenGL init
     */
    // Sets background color.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enables Z buffer.
    glEnable(GL_DEPTH_TEST);

    // Culling.
    glEnable(GL_CULL_FACE);

    // For drawing thicker lines.
    glEnable(GL_LINE_WIDTH);

    /**
     * Loop variables.
     */
    // Selects which object to act upon. 0 is reserved for world, 1-5 for objects.
    int selectedObject = 0;

    GLenum renderMode = GL_TRIANGLES;

    glm::vec2 clickPosition = glm::vec2();

    // Program loop.
    while (!glfwWindowShouldClose(window)) {
        // Clears depth + color buffer.
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        /**
         * Rendering.
         */
        // Render each camera.
        for(auto camera : cameras) {
            // Binds the viewport.
            if(camera->viewport.render()) {
                // Render lines.
                for(auto line: lines) {
                    line->render(camera, GL_LINES);
                }

                // Render each object.
                for(auto object : objects) {
                    // Bind object render to camera.
                    object->render(camera, renderMode);
                }
            }
        }

        /**
         * Transform hierarchy input.
         */
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            renderMode = GL_TRIANGLES;
        }

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            renderMode = GL_LINES;
        }

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            renderMode = GL_POINTS;
        }

        /**
         * GLFW events.
         */
        glfwSwapBuffers(window);
        glfwPollEvents();

        controller->update();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    glfwTerminate();

    return 0;
}