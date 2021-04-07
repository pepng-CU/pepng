#include "renderer.hpp"

#include "../io/io.hpp"
#include "transform.hpp"
#include "../object/camera.hpp"

Renderer::Renderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum render_mode) :
    Component("Renderer"),
    model(model),
    material(material),
    render_mode(render_mode),
    receive_shadow(true),
    display_texture(true)
{}

Renderer::Renderer(const Renderer& renderer) :
    Component(renderer),
    model(renderer.model->clone2()),
    material(renderer.material->clone()),
    render_mode(renderer.render_mode),
    receive_shadow(renderer.receive_shadow),
    display_texture(renderer.display_texture)
{}

std::shared_ptr<Renderer> Renderer::make_renderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum render_mode) {
    std::shared_ptr<Renderer> renderer(new Renderer(model, material, render_mode));

    return renderer;
}

std::shared_ptr<Renderer> pepng::make_renderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum render_mode) {
    return Renderer::make_renderer(model, material, render_mode);
}

Renderer* Renderer::clone_implementation() {
    return new Renderer(*this);
}

void Renderer::render(std::shared_ptr<WithComponents> parent, GLuint shaderProgram) {
    if(!this->model->is_init()) this->model->delayed_init();

    if(this->model->vao() == -1 || !this->active()) return;

    glUseProgram(shaderProgram);
    
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, this->material->texture->gl_index());

    std::shared_ptr<Transform> transform = parent->get_component<Transform>();

    if (transform == nullptr) {
        std::stringstream ss;

        ss << *parent << " has no transform." << std::endl;

        std::cout << ss.str() << std::endl;

        throw std::runtime_error(ss.str());
    }
    
    GLuint uWorld = glGetUniformLocation(shaderProgram, "u_world");

    if (uWorld >= 0) {
        auto worldMatrix = transform->parent_matrix
            * glm::translate(glm::mat4(1.0f), this->model->offset())
            * transform->world_matrix()
            * glm::translate(glm::mat4(1.0f), -this->model->offset());

        glUniformMatrix4fv(
            uWorld,
            1,
            GL_FALSE,
            glm::value_ptr(worldMatrix)
        );
    }

    GLuint uReceiveShadow = glGetUniformLocation(shaderProgram, "u_receive_shadow");

    if(uReceiveShadow >= 0) {
        glUniform1f(
            uReceiveShadow,
            this->receive_shadow
        );
    }

    GLuint uDisplayTexture = glGetUniformLocation(shaderProgram, "u_display_texture");

    if(uDisplayTexture >= 0) {
        glUniform1f(
            uDisplayTexture,
            this->display_texture
        );
    }

    glBindVertexArray(this->model->vao());

    if(this->model->has_element_array()) {
        glDrawElements(
            this->render_mode,
            this->model->count(),
            GL_UNSIGNED_INT,
            0
        );
    } else {
        glDrawArrays(
            this->render_mode,
            0,
            this->model->count()
        );
    }
}

void Renderer::render(std::shared_ptr<WithComponents> parent) {
    auto shaderProgram = this->material->shader_program();

    glUseProgram(shaderProgram);

    if(Camera::current_camera == nullptr) {
        std::cout << "No current camera set." << std::endl;

        throw std::runtime_error("No current camera set.");
    }

    Camera::current_camera->render(shaderProgram);

    /**
     * TODO: This is a hacky way to select the current light.
     */
    if(Light::lights.size() > 0) {
        Light::lights.at(0)->render(shaderProgram);
    }

    this->render(parent, shaderProgram);
}

#ifdef IMGUI
void Renderer::imgui() {
    Component::imgui();

    ImGui::LabelText("Name", this->model->name().c_str());

    std::stringstream ss;

    ss << this->model->count();

    ImGui::LabelText("Index Count", ss.str().c_str());

    const char* items[] = { "Lines", "Points", "Triangles" };
    static int item_current_idx = -1;

    if (item_current_idx == -1) {
        switch(this->render_mode) {
            case GL_TRIANGLES:
                item_current_idx = 2;
                break;
            case GL_POINTS:
                item_current_idx = 1;
                break;
            case GL_LINES:
                item_current_idx = 0;
        }
    }

    const char* combo_label = items[item_current_idx];

    if (ImGui::BeginCombo("Mode", combo_label)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
            
            if (ImGui::Selectable(items[n], is_selected)) {
                item_current_idx = n;

                switch(item_current_idx) {
                    case 2:
                        this->render_mode = GL_TRIANGLES;
                        break;
                    case 1:
                        this->render_mode = GL_POINTS;
                        break;
                    case 0:
                        this->render_mode = GL_LINES;
                        break;
                }
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Checkbox("Texture", &this->display_texture);
    ImGui::Checkbox("Shadow", &this->receive_shadow);
}
#endif