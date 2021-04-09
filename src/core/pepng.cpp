#include "pepng.hpp"

#include "../../src/component/camera.hpp"
#include "../../src/component/light.hpp"
#include "../../src/gl/texture.hpp"
#include "../util/load.hpp"

namespace pepng {
    static GLFWwindow *WINDOW = 0;
    static std::shared_ptr<Input> INPUT;
    static std::vector<std::shared_ptr<Object>> WORLD;
    static std::shared_ptr<Object> CURRENT_IMGUI_OBJECT;
    static glm::vec3 BACKGROUND_COLOR;

    static float WINDOW_X;
    static float WINDOW_Y;

    GLFWwindow *window() { return WINDOW; }

    std::shared_ptr<Input> input() { return INPUT; }

    std::vector<std::shared_ptr<Object>> world() { return WORLD; }

    float windowX() { return WINDOW_X; }

    float windowY() { return WINDOW_Y; }
}

void windowSizeCallback(GLFWwindow *window, int width, int height) {
    pepng::WINDOW_X = width;
    pepng::WINDOW_Y = height;
}

void pepng::set_background_color(glm::vec3 color) { BACKGROUND_COLOR = color; }

void pepng::set_object_shader(GLuint shader_program) {
    glUseProgram(shader_program);

    glUniform1i(glGetUniformLocation(shader_program, "u_texture"), 0);
    glUniform1i(glGetUniformLocation(shader_program, "u_shadow"), 1);

    pepng::load_set_object_shader(shader_program);
}

void pepng::set_shadow_shader(GLuint shader_program) {
    pepng::load_set_shadow_shader(shader_program);
}

void pepng::set_missing_texture(const std::filesystem::path& filePath) {
    static auto missing_texture = pepng::make_texture(filePath);

    missing_texture->delayed_init();
}

void pepng::set_window_icon(const std::filesystem::path& filePath) {
    #ifdef _MSC_VER
        const std::string& filePathString = (const std::string&) filePath.u8string();
    #else
        const std::string& filePathString = (const std::string&) filePath;
    #endif

    GLFWimage images[1];
    images[0].pixels = stbi_load(filePathString.c_str(), &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(pepng::window(), 1, images); 
    stbi_image_free(images[0].pixels);
}

void pepng::attach_device(std::shared_ptr<Device> device) {
    INPUT->attach_device(device);
}

void pepng::instantiate(std::shared_ptr<Object> object) {
    WORLD.push_back(object);
}

#ifdef IMGUI
namespace pepng {
    void imgui_init() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(pepng::window(), false);
        ImGui_ImplOpenGL3_Init();

        glfwSetMouseButtonCallback(pepng::window(), ImGui_ImplGlfw_MouseButtonCallback);
        glfwSetScrollCallback(pepng::window(), ImGui_ImplGlfw_ScrollCallback);
        glfwSetKeyCallback(pepng::window(), ImGui_ImplGlfw_KeyCallback);
        glfwSetCharCallback(pepng::window(), ImGui_ImplGlfw_CharCallback);
    }

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

    void imgui_render() {
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

        ImGui::Begin("Texture");

        static int index = 1;

        ImGui::InputInt("Index", &index);

        ImGui::Image((void*)(intptr_t)index, ImVec2(256, 256));

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
#endif

bool pepng::init(const char *title, float width, float height) {
    srand(time(NULL));

    pepng::WINDOW_X = width;
    pepng::WINDOW_Y = height;

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
    #ifdef IMGUI
    pepng::imgui_init();
    #endif

    /**
     * Input
     */
    pepng::INPUT = pepng::make_input(pepng::WINDOW);

    /**
     * OpenGL
     */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void pepng::extra::update_objects() {
    for(auto object : WORLD) {
        object->update();
    }
}

void pepng::extra::render_shadows() {
    for(auto light : Light::lights) {
        if(light->active()) {
            light->init_fbo();

            for(auto object : WORLD) {
                object->render(light->shader_program);
            }

            light->update_fbo();
        }
    }
}

void pepng::extra::render_objects() {
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
}

void pepng::extra::render_imgui() {
    #ifdef IMGUI
    pepng::imgui_render();
    #endif
}

void pepng::extra::update_glfw() {
    glfwSwapBuffers(pepng::window());
    glfwPollEvents();
}

namespace pepng {
    void do_frame() {
        pepng::extra::update_objects();

        pepng::extra::render_shadows();

        pepng::extra::render_objects();

        pepng::extra::render_imgui();

        pepng::extra::update_glfw();
    }
}

int pepng::update(void (*do_frame)()) {
    glClearColor(BACKGROUND_COLOR.x, BACKGROUND_COLOR.y, BACKGROUND_COLOR.z, 1.0f);

    if (do_frame == nullptr) {
        do_frame = pepng::do_frame;
    }

    #ifdef EMSCRIPTEN
        emscripten_set_main_loop(do_frame, 0, 1);
    #else
        while(!glfwWindowShouldClose(WINDOW)) {
            (*do_frame)();
        }
    #endif

    glfwTerminate();

    return 0;
}