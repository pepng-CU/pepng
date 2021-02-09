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
#include "component.hpp"
#include "io.hpp"

void objectHierarchy(std::shared_ptr<Object> object) {
    if(ImGui::TreeNode(object->model->name.c_str())) {
        for(auto child : object->children) {
            objectHierarchy(child);
        }

        ImGui::TreePop();
    }
}

static glm::vec2 windowDimension = glm::vec2(1024.0f, 768.0f);

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    windowDimension = glm::vec2(width, height);
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

    GLFWwindow *window = glfwCreateWindow(windowDimension.x, windowDimension.y, "Comp371 - Final Project", NULL, NULL);

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
    glfwSetWindowSizeCallback(window, windowSizeCallback);

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
                glm::vec2(1.0f, 1.0f)
            },
            std::make_shared<Perspective>(
                glm::radians(60.0f),
                windowDimension.x / windowDimension.y,
                0.01f,
                1000.0f
            )
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
    auto currentObject = objects.at(0);
    auto renderMode = std::make_shared<GLenum>(GL_TRIANGLES);

    cameras.at(0)->attach(std::make_shared<FPSComponent>());
    currentObject->attach(std::make_shared<MovementComponent>());
    currentObject->attach(std::make_shared<RenderModeComponent>(renderMode));

    auto input = Input::makeInput(window)
        ->attach(
            Device::makeDevice(DeviceType::MOUSE)
                ->attach(Axis::makeAxis("mouseY", AxisType::FIRST))
                ->attach(Axis::makeAxis("mouseX", AxisType::SECOND))
                ->attach(Button::makeButton("zoom", GLFW_MOUSE_BUTTON_5))
                ->attach(Button::makeButton("pan", GLFW_MOUSE_BUTTON_MIDDLE))
                ->attach(Button::makeButton("pan", GLFW_MOUSE_BUTTON_4))
                ->attach(Button::makeButton("rotate", GLFW_MOUSE_BUTTON_RIGHT))
        )
        ->attach(
            Device::makeDevice(DeviceType::KEYBOARD)
                ->attach(Button::makeButton("vertical", GLFW_KEY_W))
                ->attach(Button::makeButton("vertical", GLFW_KEY_S, -1.0f))
                ->attach(Button::makeButton("horizontal", GLFW_KEY_A))
                ->attach(Button::makeButton("horizontal", GLFW_KEY_D, -1.0f))
                ->attach(Button::makeButton("yaw", GLFW_KEY_UP))
                ->attach(Button::makeButton("yaw", GLFW_KEY_DOWN, -1.0f))
                ->attach(Button::makeButton("pitch", GLFW_KEY_LEFT))
                ->attach(Button::makeButton("pitch", GLFW_KEY_RIGHT, -1.0f))
                ->attach(Button::makeButton("triangles", GLFW_KEY_T))
                ->attach(Button::makeButton("points", GLFW_KEY_P))
                ->attach(Button::makeButton("lines", GLFW_KEY_L))
        );

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
            if(camera->viewport.render(windowDimension)) {
                camera->projection->setAspect(windowDimension.x / windowDimension.y);
                
                camera->update();

                for(auto line: lines) {
                    line->render(camera, GL_LINES);
                }

                for(auto object : objects) {
                    object->render(camera, *renderMode);
                    object->update();
                }
            }
        }

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

        ImGui::LabelText("Name", currentObject->model->name.c_str());

        if(ImGui::CollapsingHeader("Transform")) {
            glm::vec3 rotation = glm::degrees(currentObject->getEuler());

            ImGui::InputFloat3("Position", glm::value_ptr(currentObject->position));
            ImGui::InputFloat3("Rotation", glm::value_ptr(rotation));
            ImGui::InputFloat3("Scale", glm::value_ptr(currentObject->scale));
        }

        for(auto component : currentObject->components) {
            if(ImGui::CollapsingHeader(component->getName().c_str())) {
                component->imgui();
            }
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
        } else if(ImGui::Button("Create Sponza")) {
            auto object = Object::fromOBJ(modelpath / "sponza.obj", shaderProgram, Transform {
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 1.0f, 1.0f)
            });

            objects.push_back(object);
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