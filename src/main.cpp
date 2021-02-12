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
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "util/utils.hpp"
#include "util/load.hpp"
#include "gl/gl.hpp"
#include "component/components.hpp"
#include "object/objects.hpp"
#include "io/io.hpp"
#include "ui/ui.hpp"

static glm::vec2 windowDimension = glm::vec2(1024.0f, 768.0f);

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    windowDimension = glm::vec2(width, height);
}

void objectHierarchy(std::shared_ptr<Object> object, std::shared_ptr<Object>* currentObject) {
    ImGuiTreeNodeFlags nodeFlags = 0;

    if(object == *currentObject) {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)&object, nodeFlags, object->name.c_str());

    if (ImGui::IsItemClicked()) {
        *currentObject = object;
    }

    if(nodeOpen) {
        for(auto child : object->children) {
            objectHierarchy(child, currentObject);
        }

        ImGui::TreePop();
    }
}

void objectAttachRecursive(std::shared_ptr<Object> object) {
    object->attachComponent(pepng::makeTransformer());

    for(auto child : object->children) {
        objectAttachRecursive(child);
    }
}

static std::shared_ptr<Object> currentObject = nullptr;

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
    auto texturespath = pepng::getFolderPath("textures");

    static auto missingTexture = pepng::makeTexture(texturespath / "missing.jpg");

    /**
     * Shaders
     */
    auto shaderpath = pepng::getFolderPath("shaders");

    static auto shaderProgram = pepng::makeShaderProgram(
        pepng::makeShader(shaderpath / "object/vertex.glsl", GL_VERTEX_SHADER),
        pepng::makeShader(shaderpath / "object/fragment.glsl", GL_FRAGMENT_SHADER)
    );

    static auto lineShaderProgram = pepng::makeShaderProgram(
        pepng::makeShader(shaderpath / "line/vertex.glsl", GL_VERTEX_SHADER),
        pepng::makeShader(shaderpath / "line/fragment.glsl", GL_FRAGMENT_SHADER)
    );

    /**
     * Objects
     */
    auto modelpath = pepng::getFolderPath("models");

    static std::vector<std::shared_ptr<Object>> objects {
        pepng::makeAxes(
            pepng::makeTransform(
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(7.0f, 7.0f, 7.0f)
            ), 
            lineShaderProgram
        ),
        pepng::makeGrid(
            pepng::makeTransform(
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(128.0f, 128.0f, 128.0f)
            ), 
            lineShaderProgram, 
            129
        )
    };

    pepng::load(
        modelpath / "sponza/scene.dae", 
        std::function([](std::shared_ptr<Object> object) {
            object->attachComponent(pepng::makeSelector());

            objectAttachRecursive(object);

            objects.push_back(object);
        }),
        shaderProgram,
        pepng::makeTransform()
    );

    /**
     * Controller
     */
    auto input = pepng::makeInput(window);

    auto mouse = pepng::makeDevice(DeviceType::MOUSE)
        ->attachUnit(pepng::makeAxis("mouseY", AxisType::FIRST))
        ->attachUnit(pepng::makeAxis("mouseX", AxisType::SECOND))
        ->attachUnit(pepng::makeButton("zoom", GLFW_MOUSE_BUTTON_5))
        ->attachUnit(pepng::makeButton("pan", GLFW_MOUSE_BUTTON_MIDDLE))
        ->attachUnit(pepng::makeButton("pan", GLFW_MOUSE_BUTTON_4))
        ->attachUnit(pepng::makeButton("rotate", GLFW_MOUSE_BUTTON_RIGHT));

    auto keyboard = pepng::makeDevice(DeviceType::KEYBOARD)
        ->attachUnit(pepng::makeButton("vertical", GLFW_KEY_W))
        ->attachUnit(pepng::makeButton("vertical", GLFW_KEY_S, -1.0f))
        ->attachUnit(pepng::makeButton("horizontal", GLFW_KEY_A))
        ->attachUnit(pepng::makeButton("horizontal", GLFW_KEY_D, -1.0f))
        ->attachUnit(pepng::makeButton("yaw", GLFW_KEY_UP))
        ->attachUnit(pepng::makeButton("yaw", GLFW_KEY_DOWN, -1.0f))
        ->attachUnit(pepng::makeButton("pitch", GLFW_KEY_LEFT))
        ->attachUnit(pepng::makeButton("pitch", GLFW_KEY_RIGHT, -1.0f))
        ->attachUnit(pepng::makeButton("triangles", GLFW_KEY_T))
        ->attachUnit(pepng::makeButton("points", GLFW_KEY_P))
        ->attachUnit(pepng::makeButton("lines", GLFW_KEY_L))
        ->attachUnit(pepng::makeButton("recenter", GLFW_KEY_HOME))
        ->attachUnit(pepng::makeButton("scale", GLFW_KEY_U))
        ->attachUnit(pepng::makeButton("scale", GLFW_KEY_J, -1.0f));

    for(int i = 0; i < 10; i++) {
        std::stringstream ss;

        ss << "object_" << i;

        keyboard->attachUnit(pepng::makeButton(ss.str(), GLFW_KEY_0 + i));
    }

    input
        ->attachDevice(keyboard)
        ->attachDevice(mouse);

    /**
     * OpenGL
     */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        /**
         * Update
         */
        for(auto camera : Camera::cameras) {
            if(camera->isActive) {
                camera->viewport->render(windowDimension);

                Camera::currentCamera = camera;

                camera->projection->setAspect(windowDimension.x / windowDimension.y);

                for(auto object : objects) {
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
            objectHierarchy(object, &currentObject);
        }

        ImGui::End();

        ImGui::Begin("Inspector");

        if(currentObject) {
            currentObject->imgui();
        }
        
        ImGui::End();

        ImGui::Begin("Debug");

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