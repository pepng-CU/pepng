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

#include <pepng.h>

static glm::vec2 windowDimension = glm::vec2(1024.0f, 768.0f);

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    windowDimension = glm::vec2(width, height);
}

void object_hierarchy(std::shared_ptr<Object> object, std::shared_ptr<Object>* currentObject) {
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
            object_hierarchy(child, currentObject);
        }

        ImGui::TreePop();
    }
}

void object_attach_recursive(std::shared_ptr<Object> object) {
    object->attach_component(pepng::make_transformer());

    // Adds DynamicTexture to the screen.
    if(object->name == "Display") {
        object->attach_component(pepng::make_dynamic_texture(2, 4));
    }

    for(auto child : object->children) {
        object_attach_recursive(child);
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
     * Paths
     */
    auto texturespath = pepng::get_folder_path("textures");
    auto modelpath = pepng::get_folder_path("models");
    auto shaderpath = pepng::get_folder_path("shaders");

    /**
     * Textures
     */

    static auto missingTexture = pepng::make_texture(texturespath / "missing.jpg");
    missingTexture->delayed_init();

    // Loadings screen for stage.
    for(int i = 1; i <= 3; i++) {
        pepng::make_texture(modelpath / "pa2" / "screens" / (std::to_string(i) + ".jpg"))->delayed_init();
    }

    /**
     * Shaders
     */
    static auto shaderProgram = pepng::make_shader_program(
        pepng::make_shader(shaderpath / "object" / "vertex.glsl", GL_VERTEX_SHADER),
        pepng::make_shader(shaderpath / "object" / "fragment.glsl", GL_FRAGMENT_SHADER)
    );

    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "u_texture"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "u_shadow"), 1);

    static auto lineShaderProgram = pepng::make_shader_program(
        pepng::make_shader(shaderpath / "line" / "vertex.glsl", GL_VERTEX_SHADER),
        pepng::make_shader(shaderpath / "line" / "fragment.glsl", GL_FRAGMENT_SHADER)
    );

    static auto shadowShaderProgram = pepng::make_shader_program(
        pepng::make_shader(shaderpath / "shadow" / "vertex.glsl", GL_VERTEX_SHADER),
        pepng::make_shader(shaderpath / "shadow" / "fragment.glsl", GL_FRAGMENT_SHADER),
        pepng::make_shader(shaderpath / "shadow" / "geometry.glsl", GL_GEOMETRY_SHADER)
    );

    static auto textureShaderProgram = pepng::make_shader_program(
        pepng::make_shader(shaderpath / "texture" / "vertex.glsl", GL_VERTEX_SHADER),
        pepng::make_shader(shaderpath / "texture" / "fragment.glsl", GL_FRAGMENT_SHADER)
    );

    /**
     * Objects
     */
    static std::vector<std::shared_ptr<Object>> objects {
        pepng::make_axes(
            pepng::make_transform(
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(7.0f, 7.0f, 7.0f)
            ), 
            lineShaderProgram
        ),
        pepng::make_grid(
            pepng::make_transform(
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(128.0f, 128.0f, 128.0f)
            ), 
            lineShaderProgram, 
            129
        )
    };

    pepng::load(
        modelpath / "sponza" / "scene.dae", 
        std::function([](std::shared_ptr<Object> object) {
            object->attach_component(pepng::make_selector());

            object_attach_recursive(object);

            objects.push_back(object);
        }),
        shaderProgram,
        pepng::make_transform(),
        shadowShaderProgram
    );

    /**
     * Controller
     */
    auto input = pepng::make_input(window);

    auto mouse = pepng::make_device(DeviceType::MOUSE)
        ->attach_unit(pepng::make_axis("mouseY", AxisType::FIRST))
        ->attach_unit(pepng::make_axis("mouseX", AxisType::SECOND))
        ->attach_unit(pepng::make_axis("zoom", AxisType::THIRD, 25.0f, true))
        ->attach_unit(pepng::make_button("pan", GLFW_MOUSE_BUTTON_MIDDLE))
        ->attach_unit(pepng::make_button("pan", GLFW_MOUSE_BUTTON_4))
        ->attach_unit(pepng::make_button("rotate", GLFW_MOUSE_BUTTON_RIGHT));

    auto keyboard = pepng::make_device(DeviceType::KEYBOARD)
        ->attach_unit(pepng::make_button("vertical", GLFW_KEY_W))
        ->attach_unit(pepng::make_button("vertical", GLFW_KEY_S, -1.0f))
        ->attach_unit(pepng::make_button("horizontal", GLFW_KEY_A))
        ->attach_unit(pepng::make_button("horizontal", GLFW_KEY_D, -1.0f))
        ->attach_unit(pepng::make_button("svertical", GLFW_KEY_Q))
        ->attach_unit(pepng::make_button("svertical", GLFW_KEY_E, -1.0f))
        ->attach_unit(pepng::make_button("shorizontal", GLFW_KEY_C))
        ->attach_unit(pepng::make_button("shorizontal", GLFW_KEY_V, -1.0f))
        ->attach_unit(pepng::make_button("yaw", GLFW_KEY_UP))
        ->attach_unit(pepng::make_button("yaw", GLFW_KEY_DOWN, -1.0f))
        ->attach_unit(pepng::make_button("pitch", GLFW_KEY_LEFT))
        ->attach_unit(pepng::make_button("pitch", GLFW_KEY_RIGHT, -1.0f))
        ->attach_unit(pepng::make_button("triangles", GLFW_KEY_T))
        ->attach_unit(pepng::make_button("points", GLFW_KEY_P))
        ->attach_unit(pepng::make_button("lines", GLFW_KEY_L))
        ->attach_unit(pepng::make_button("recenter", GLFW_KEY_HOME))
        ->attach_unit(pepng::make_button("shadow", GLFW_KEY_B))
        ->attach_unit(pepng::make_button("texture", GLFW_KEY_X))
        ->attach_unit(pepng::make_button("scale", GLFW_KEY_U))
        ->attach_unit(pepng::make_button("scale", GLFW_KEY_J, -1.0f));

    for(int i = 0; i < 10; i++) {
        std::stringstream ss;

        ss << "object_" << i;

        keyboard->attach_unit(pepng::make_button(ss.str(), GLFW_KEY_0 + i));
    }

    input
        ->attach_device(keyboard)
        ->attach_device(mouse);

    auto textureObj = pepng::make_texture_object(pepng::make_transform(), textureShaderProgram);

    /**
     * OpenGL
     */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while(!glfwWindowShouldClose(window)) {
        /**
         * Update
         */
        for(auto object : objects) {
            object->update();
        }

        /**
         * Shadow
         */
        for(auto light : Light::lights) {
            if(light->active()) {
                light->init_fbo();

                for(auto object : objects) {
                    object->render(light->shader_program);
                }

                light->update_fbo();
            }
        }

        /**
         * Render
         */
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        for(auto camera : Camera::cameras) {
            if(camera->active()) {
                camera->viewport->render(windowDimension);

                Camera::current_camera = camera;

                camera->projection->set_aspect(windowDimension.x / windowDimension.y);

                for(auto object : objects) {
                    object->render();
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
            object_hierarchy(object, &currentObject);
        }

        ImGui::End();

        ImGui::Begin("Inspector");

        if(currentObject) {
            currentObject->imgui();
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