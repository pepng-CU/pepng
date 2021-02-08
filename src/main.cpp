/**
 * COMP371 Final Project
 * 
 * @author Alexandre Lavoie (40129457), Antoine Poulin (40131102), Rhys Rosenberg (400719895), Arman Jahanpour (40075912)
 */

#define GLEW_STATIC 1
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <algorithm>
#include <filesystem>
#include <stdlib.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "shader.hpp"
#include "model.hpp"
#include "utils.hpp"
#include "camera.hpp"
#include "transform.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "controller.hpp"
#include "component.hpp"

int main(int argc, char *argv[]) {
    srand(time(NULL));

    /**
     * GLFW
     */
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

    glfwSwapInterval(1);

    /**
     * ImGui
     */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

    /**
     * Textures
     */
    auto texturespath = utils::getPath("textures");

    createTexture(texturespath / "honeycomb.jpg");

    /**
     * Shaders
     */
    auto shaderpath = utils::getPath("shaders");

    GLuint shaderProgram = ShaderBuilder()
        .attach(compileShader(readShader(shaderpath / "object/vertex.glsl"), GL_VERTEX_SHADER))
        .attach(compileShader(readShader(shaderpath / "object/fragment.glsl"), GL_FRAGMENT_SHADER))
        .build();

    GLuint lineShaderProgram = ShaderBuilder()
        .attach(compileShader(readShader(shaderpath / "line/vertex.glsl"), GL_VERTEX_SHADER))
        .attach(compileShader(readShader(shaderpath / "line/fragment.glsl"), GL_FRAGMENT_SHADER))
        .build();

    /**
     * Models
     */
    auto modelpath = utils::getPath("models");

    std::vector<std::shared_ptr<Object>> objects {
        Object::fromOBJ(modelpath / "scene.obj", shaderProgram, Transform {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        }),
        Object::fromOBJ(modelpath / "sponza.obj", shaderProgram, Transform {
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
     * Controller + Components
     */
    auto controller = std::make_shared<Controller>(window);

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
    controller->attach(std::make_shared<EscapeComponent>());

    auto renderMode = std::make_shared<GLenum>(GL_TRIANGLES);
    controller->attach(std::make_shared<RenderModeComponent>(renderMode));

    /**
     * OpenGL
     */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        /**
         * Rendering.
         */
        for(auto camera : cameras) {
            if(camera->viewport.render()) {
                for(auto line: lines) {
                    line->render(camera, GL_LINES);
                }

                for(auto object : objects) {
                    object->render(camera, *renderMode);
                }
            }
        }

        // Update input.
        controller->update();

        /**
         * GLFW events.
         */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}