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

void objectHierarchy(std::shared_ptr<Object> object) {
    if(ImGui::TreeNode(object->model->name.c_str())) {
        for(auto child : object->children) {
            objectHierarchy(child);
        }

        ImGui::TreePop();
    }
}

void objectMovementComponent(std::shared_ptr<Object> object, std::vector<std::shared_ptr<MovementComponent>>* list) {
    list->push_back(
        std::make_shared<MovementComponent>(
            object
        )
    );

    for(auto child : object->children) {
        objectMovementComponent(child, list);
    }
}

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

    std::vector<std::shared_ptr<MovementComponent>> objectComponents;

    for(auto object : objects) {
        objectMovementComponent(object, &objectComponents);
    }

    auto objectManager = std::make_shared<ObjectManagerComponent>(objectComponents);
    controller->attach(objectManager); 
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
         * ImGui
         */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hierarchy");
        
        for(auto object : objects) {
            objectHierarchy(object);
        }

        ImGui::End();

        ImGui::Begin("Inspector");
        // TODO: Check if it is actually an object...
        auto currentObject = std::static_pointer_cast<Object>(objectManager->getCurrentComponent()->getTransform());

        ImGui::LabelText("Name", currentObject->model->name.c_str());

        if(ImGui::CollapsingHeader("Transform")) {
            glm::vec3 rotation = glm::degrees(currentObject->getEuler());

            ImGui::InputFloat3("Position", glm::value_ptr(currentObject->position));
            ImGui::InputFloat3("Rotation", glm::value_ptr(rotation));
            ImGui::InputFloat3("Scale", glm::value_ptr(currentObject->scale));
        }

        ImGui::End();

        ImGui::Begin("Debug");

        if(ImGui::Button("Create Cube")) {
            auto object = Object::fromOBJ(modelpath / "cube.obj", shaderProgram, Transform {
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 1.0f, 1.0f)
            });

            objects.push_back(object);

            objectManager->components.push_back(std::make_shared<MovementComponent>(object));
        } else if(ImGui::Button("Create Sponza")) {
            auto object = Object::fromOBJ(modelpath / "sponza.obj", shaderProgram, Transform {
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 1.0f, 1.0f)
            });

            objects.push_back(object);

            objectManager->components.push_back(std::make_shared<MovementComponent>(object));
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /**
         * GLFW events.
         */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}