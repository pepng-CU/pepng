#include <pepng.h>

int main(int argc, char *argv[]) {
    if (!pepng::init("COMP 371", 1024, 768)) return -1;

    /**
     * Paths
     */
    auto texture_path = pepng::get_folder_path("textures");
    auto model_path = pepng::get_folder_path("models");
    auto shader_path = pepng::get_folder_path("shaders");

    /**
     * Textures
     */
    pepng::set_missing_texture(texture_path / "missing.jpg");

    // Load screens for stage.
    for(int i = 1; i <= 3; i++) {
        pepng::make_texture(model_path / "pa2" / "screens" / (std::to_string(i) + ".jpg"))->delayed_init();
    }

    /**
     * Shaders
     */
    auto object_shader_program = pepng::make_shader_program(
        pepng::make_shader(shader_path / "object" / "vertex.glsl", GL_VERTEX_SHADER),
        pepng::make_shader(shader_path / "object" / "fragment.glsl", GL_FRAGMENT_SHADER)
    );

    pepng::set_object_shader(object_shader_program);

    auto line_shader_program = pepng::make_shader_program(
        pepng::make_shader(shader_path / "line" / "vertex.glsl", GL_VERTEX_SHADER),
        pepng::make_shader(shader_path / "line" / "fragment.glsl", GL_FRAGMENT_SHADER)
    );

    auto shadow_shader_program = pepng::make_shader_program(
        pepng::make_shader(shader_path / "shadow" / "vertex.glsl", GL_VERTEX_SHADER),
        pepng::make_shader(shader_path / "shadow" / "fragment.glsl", GL_FRAGMENT_SHADER),
        pepng::make_shader(shader_path / "shadow" / "geometry.glsl", GL_GEOMETRY_SHADER)
    );

    pepng::set_shadow_shader(shadow_shader_program);

    /**
     * Objects
     */
    pepng::instantiate(pepng::make_axes(
        pepng::make_transform(
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(7.0f, 7.0f, 7.0f)
        ), 
        line_shader_program
    ));

    pepng::instantiate(
        pepng::make_grid(
            pepng::make_transform(
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(128.0f, 128.0f, 128.0f)
            ), 
            line_shader_program, 
            129
        )
    );

    pepng::load_file(
        model_path / "sponza" / "scene.dae", 
        std::function([](std::shared_ptr<Object> object) {
            object->attach_component(pepng::make_selector());

            /**
             * Binds components to loaded objects.
             */
            object->for_each([](std::shared_ptr<Object> obj) {
                obj->attach_component(pepng::make_transformer());

                // Adds FPS controller if camera.
                if(obj->has_component<Camera>()) {
                    obj->attach_component(pepng::make_fps());
                }

                // Adds DynamicTexture if object named Display (in this case, the screen).
                if(obj->name == "Display") {
                    obj->attach_component(pepng::make_dynamic_texture(2, 4));
                }
            });

            pepng::instantiate(object);
        }),
        pepng::make_transform()
    );

    /**
     * Controllers
     */
    auto mouse = pepng::make_device(DeviceType::MOUSE)
        ->attach_unit(pepng::make_axis("mouseY", AxisType::FIRST))
        ->attach_unit(pepng::make_axis("mouseX", AxisType::SECOND))
        ->attach_unit(pepng::make_axis("zoom", AxisType::THIRD, 25.0f, true))
        ->attach_unit(pepng::make_button("pan", GLFW_MOUSE_BUTTON_MIDDLE))
        ->attach_unit(pepng::make_button("pan", GLFW_MOUSE_BUTTON_4))
        ->attach_unit(pepng::make_button("rotate", GLFW_MOUSE_BUTTON_RIGHT));

    pepng::attach_device(mouse);

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

    pepng::attach_device(keyboard);

    return pepng::update();
}