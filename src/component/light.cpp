#include "light.hpp"

int Light::__count = 0;
std::vector<std::shared_ptr<Light>> Light::lights;

Light::Light(GLuint shader_program, glm::vec3 color, float intensity) : 
    Component("Light"),
    _shader_program(shader_program),
    _near(0.1f),
    _far(100.0f),
    _intensity(intensity),
    _color(color),
    _shadows(true),
    _texture_index(Light::__count++)
{}

Light::Light(const Light& light) : 
    Component(light),
    _shader_program(light._shader_program),
    _near(light._near),
    _far(light._far),
    _intensity(light._intensity),
    _color(light._color),
    _shadows(light._shadows),
    _texture_index(Light::__count++)
{}

void Light::init(std::shared_ptr<WithComponents> parent) {
    this->_transform = parent->get_component<Transform>();

    if(this->_transform == nullptr) {
        std::stringstream ss;

        ss << "Light does not have a transform.";

        std::cout << ss.str() << std::endl;

        throw std::runtime_error(ss.str());
    }
}

void Light::update_fbo() {
    #ifdef EMSCRIPTEN
    glDrawBuffers(0, nullptr);
    #else
    glDrawBuffer(GL_NONE);
    #endif
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#ifdef IMGUI
void Light::imgui() {
    Component::imgui();

    ImGui::InputFloat("Near", &this->_near);
    ImGui::InputFloat("Far", &this->_far);
    ImGui::InputFloat("Intensity", &this->_intensity);
    ImGui::ColorPicker3("Color", glm::value_ptr(this->_color));
}
#endif