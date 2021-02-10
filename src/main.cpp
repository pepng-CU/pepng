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

void objectAttachTransformer(std::shared_ptr<Object> object) {
    object->attach(std::make_shared<Transformer>());

    for(auto child : object->children) {
        objectAttachTransformer(child);
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
    auto texturespath = utils::getPath("textures");

    static auto missingTexture = createTexture(texturespath / "missing.jpg");
    static auto honeyTexture = createTexture(texturespath / "honeycomb.jpg");
    static auto objectTexture = createTexture(texturespath / "suzanne.png");

    /**
     * Shaders
     */
    auto shaderpath = utils::getPath("shaders");

    static GLuint shaderProgram = ShaderBuilder()
        .attach(compileShader(readShader(shaderpath / "object/vertex.glsl"), GL_VERTEX_SHADER))
        .attach(compileShader(readShader(shaderpath / "object/fragment.glsl"), GL_FRAGMENT_SHADER))
        .build();

    static GLuint lineShaderProgram = ShaderBuilder()
        .attach(compileShader(readShader(shaderpath / "line/vertex.glsl"), GL_VERTEX_SHADER))
        .attach(compileShader(readShader(shaderpath / "line/fragment.glsl"), GL_FRAGMENT_SHADER))
        .build();

    /**
     * Cameras
     */
    static std::vector<std::shared_ptr<Camera>> cameras {
        std::make_shared<Camera>(
            std::make_shared<CameraTransform>(
                glm::vec3(0.0f, 2.0f, 10.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 1.0f, 1.0f)
            ),
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
     * Objects
     */
    auto modelpath = utils::getPath("models");

    static std::vector<std::shared_ptr<Object>> objects {
        std::static_pointer_cast<Object>(
            std::make_shared<Axes>(
                std::make_shared<Transform>(
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(7.0f, 7.0f, 7.0f)
                )
            , lineShaderProgram)
        ),
        std::static_pointer_cast<Object>(
            std::make_shared<Grid>(
                std::make_shared<Transform>(
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(128.0f, 128.0f, 128.0f)
                )
            , lineShaderProgram, 129)
        )
    };

    pepng::load(
        modelpath / "scene.obj", 
        std::function<void(std::shared_ptr<Object>)>([](std::shared_ptr<Object> object) {
            object->attach(std::make_shared<Selector>());

            objectAttachTransformer(object);

            objects.push_back(object);
        }),
        shaderProgram,
        std::make_shared<Transform>()
    );

    for(auto camera : cameras) {
        objects.push_back(camera);
    }

    /**
     * Controller + Components
     */
    cameras.at(0)->attach(std::make_shared<FPS>());

    auto input = Input::makeInput(window)
        ->attach(
            Device::makeDevice(DeviceType::MOUSE)
                ->attach(Axis::makeAxis("mouseY", AxisType::FIRST))
                ->attach(Axis::makeAxis("mouseX", AxisType::SECOND))
                ->attach(Button::makeButton("zoom", GLFW_MOUSE_BUTTON_5))
                ->attach(Button::makeButton("pan", GLFW_MOUSE_BUTTON_MIDDLE))
                ->attach(Button::makeButton("pan", GLFW_MOUSE_BUTTON_4))
                ->attach(Button::makeButton("rotate", GLFW_MOUSE_BUTTON_RIGHT))
        );

    auto keyboard = Device::makeDevice(DeviceType::KEYBOARD)
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
        ->attach(Button::makeButton("recenter", GLFW_KEY_HOME))
        ->attach(Button::makeButton("scale", GLFW_KEY_U))
        ->attach(Button::makeButton("scale", GLFW_KEY_J, -1.0f));

    for(int i = 0; i < 10; i++) {
        std::stringstream ss;

        ss << "object_" << i;

        keyboard->attach(Button::makeButton(ss.str(), GLFW_KEY_0 + i));
    }

    input->attach(keyboard);

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
        for(auto camera : cameras) {
            if(camera->viewport.render(windowDimension)) {
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