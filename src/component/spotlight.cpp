/**
 * This is the standard way for defining new Component cpp for PEPNG.
 */
#include "spotlight.hpp"

/**
 * STATICS
 */
int Spotlight::__count = 0;

/**
 * CONSTRUCTORS
 * 
 * Defined accoriding to C++ standards.
 */

Spotlight::Spotlight(GLuint shader_program, float angle, glm::vec3 color, float intensity) :
    Light(shader_program, color, intensity),
    __angle(angle),
    __index(__count++)
{}

Spotlight::Spotlight(const Spotlight& spotlight) : 
    Light(spotlight),
    __angle(spotlight.__angle),
    __index(__count++)
{}

/**
 * BOILERPLATE METHODS
 * 
 * These methods are common to all components.
 * It is recommended to copy the following and modify to your Component needs.
 */

Spotlight* Spotlight::clone_implementation() {
    return new Spotlight(*this);
}

std::shared_ptr<Spotlight> Spotlight::make_spotlight(GLuint shader_program, float angle, glm::vec3 color, float intensity) {
    std::shared_ptr<Spotlight> instance(new Spotlight(shader_program, angle, color, intensity));

    Light::lights.push_back(instance);

    return instance;
}

std::shared_ptr<Spotlight> pepng::make_spotlight(GLuint shader_program, float angle, glm::vec3 color, float intensity) {
    return Spotlight::make_spotlight(shader_program, angle, color, intensity);
}

/**
 * LIFETIME METHODS
 */

glm::mat4 Spotlight::matrix() {
    return glm::perspective(
        glm::radians(this->__angle), 
        1.0f, 
        0.1f, 
        this->_far
    ) 
    * glm::lookAt(this->_transform->position, this->_transform->position - this->_transform->forward(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Spotlight::delayed_init() {
    if(this->_is_init) return;
    
    this->_is_init = true;

    glGenFramebuffers(1, &this->_fbo);

    glGenTextures(1, &this->_texture);
    glBindTexture(GL_TEXTURE_2D, this->_texture);

    #ifdef EMSCRIPTEN
    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_DEPTH_COMPONENT24, 
        1024,
        1024,
        0,
        GL_DEPTH_COMPONENT,
        GL_UNSIGNED_INT,
        NULL
    );
    #else
    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_DEPTH_COMPONENT32, 
        1024,
        1024,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        NULL
    );
    #endif

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, this->_fbo);
    #ifdef EMSCRIPTEN
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->_texture, 0);
    #else
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->_texture, 0);
    #endif

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Spotlight::init_fbo() {
    this->delayed_init();

    glBindFramebuffer(GL_FRAMEBUFFER, this->_fbo);

    glViewport(0, 0, 1024, 1024);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(this->_shader_program);

    auto matrix_location = glGetUniformLocation(this->_shader_program, "u_matrix");

    glUniformMatrix4fv(
        matrix_location,
        1,
        GL_FALSE,
        glm::value_ptr(this->matrix())
    );

    auto far_location = glGetUniformLocation(this->_shader_program, "u_far");

    glUniform1f(
        far_location,
        this->_far
    );
}

void Spotlight::render(GLuint shader_program) {
    std::stringstream ss;

    ss << "u_spotlights[" << this->__index << "]";

    auto struct_prefix = ss.str();

    glUniform1i(
        glGetUniformLocation(shader_program, (struct_prefix + ".is_active").c_str()),
        this->_is_active
    );

    if(!this->_is_active) return;

    std::stringstream shadow_texture;

    shadow_texture << "u_spotlight_shadow_textures[" << this->__index << "]";

    glActiveTexture(GL_TEXTURE1 + this->_texture_index);
    glBindTexture(GL_TEXTURE_2D, this->_texture);

    auto shadow_texture_location = glGetUniformLocation(shader_program, shadow_texture.str().c_str());

    glUniform1i(shadow_texture_location, 1 + this->_texture_index);

    auto light_position = this->_transform->position;
    auto light_direction = -this->_transform->forward();

    glUniform3fv(
        glGetUniformLocation(shader_program, (struct_prefix + ".position").c_str()),
        1,
        glm::value_ptr(light_position)
    );

    glUniform3fv(
        glGetUniformLocation(shader_program, (struct_prefix + ".direction").c_str()),
        1,
        glm::value_ptr(light_direction)
    );

    glUniform1f(
        glGetUniformLocation(shader_program, (struct_prefix + ".range").c_str()),
        this->_far
    );

    glUniform3fv(
        glGetUniformLocation(shader_program, (struct_prefix + ".color").c_str()),
        1,
        glm::value_ptr(this->_color)
    );

    glUniform1f(
        glGetUniformLocation(shader_program, (struct_prefix + ".angle").c_str()),
        glm::radians(this->__angle / 2.0f)
    );

    glUniform1f(
        glGetUniformLocation(shader_program, (struct_prefix + ".intensity").c_str()),
        this->_intensity
    );

    glUniform1f(
        glGetUniformLocation(shader_program, (struct_prefix + ".shadows").c_str()),
        this->_shadows
    );

    auto matrix_location = glGetUniformLocation(shader_program, (struct_prefix + ".matrix").c_str());

    glUniformMatrix4fv(
        matrix_location,
        1,
        GL_FALSE,
        glm::value_ptr(this->matrix())
    );
}

/**
 * IMGUI
 * 
 * We need to check if IMGUI is present.
 */

#ifdef IMGUI
void Spotlight::imgui() {
    Light::imgui();

    ImGui::InputFloat("Angle", &this->__angle);
}
#endif