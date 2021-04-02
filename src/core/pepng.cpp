#include "pepng.hpp"

#include "../../src/component/camera.hpp"
#include "../../src/component/light.hpp"

namespace pepng {
    static GLFWwindow *WINDOW;
    static std::shared_ptr<Input> INPUT;
    static std::vector<std::shared_ptr<Object>> WORLD;
    static std::shared_ptr<Object> CURRENT_IMGUI_OBJECT;

    static float WINDOW_X = 1024.0f;
    static float WINDOW_Y = 768.0f;

    GLFWwindow *window() { return WINDOW; }

    /**
     * Accessor for input.
     */
    std::shared_ptr<Input> input() { return INPUT; }

    /**
     * Accessor for windowX.
     */
    float windowX() { return WINDOW_X; }

    /**
     * Accessor for windowY.
     */
    float windowY() { return WINDOW_Y; }
}

void windowSizeCallback(GLFWwindow *window, int width, int height) {
    pepng::WINDOW_X = width;
    pepng::WINDOW_Y = height;
}

void pepng::attach_device(std::shared_ptr<Device> device) {
    INPUT->attach_device(device);
}

void pepng::instantiate(std::shared_ptr<Object> object) {
    WORLD.push_back(object);
}

bool pepng::init(const char *title) {
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

    pepng::WINDOW = glfwCreateWindow(pepng::WINDOW_X, pepng::WINDOW_Y, title, NULL, NULL);

    if (pepng::window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(pepng::WINDOW);

    glewExperimental = true;

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwSwapInterval(1);
    glfwSetWindowSizeCallback(pepng::WINDOW, windowSizeCallback);

    /**
     * ImGui
     */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(pepng::WINDOW, true);
    ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

    /**
     * Input
     */
    pepng::INPUT = pepng::make_input(pepng::WINDOW);

    /**
     * OpenGL
     */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

namespace pepng {
    void object_hierarchy(std::shared_ptr<Object> object) {
        ImGuiTreeNodeFlags nodeFlags = 0;

        if(object == CURRENT_IMGUI_OBJECT) {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        }

        bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)&object, nodeFlags, object->name.c_str());

        if (ImGui::IsItemClicked()) {
            CURRENT_IMGUI_OBJECT = object;
        }

        if(nodeOpen) {
            for(auto child : object->children) {
                object_hierarchy(child);
            }

            ImGui::TreePop();
        }
    }
}

int pepng::update() {
    while(!glfwWindowShouldClose(WINDOW)) {
        /**
         * Update
         */
        for(auto object : WORLD) {
            object->update();
        }

        /**
         * Shadow
         */
        for(auto light : Light::lights) {
            if(light->active()) {
                light->init_fbo();

                for(auto object : WORLD) {
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
                camera->viewport->render(glm::vec2(WINDOW_X, WINDOW_Y));

                Camera::current_camera = camera;

                camera->projection->set_aspect(WINDOW_X / WINDOW_Y);

                for(auto object : WORLD) {
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
        
        for(auto object : WORLD) {
            object_hierarchy(object);
        }

        ImGui::End();

        ImGui::Begin("Inspector");

        if(CURRENT_IMGUI_OBJECT) {
            CURRENT_IMGUI_OBJECT->imgui();
        }
        
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /**
         * GLFW events.
         */
        glfwSwapBuffers(pepng::window());
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}