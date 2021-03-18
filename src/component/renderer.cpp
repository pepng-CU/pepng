#include "renderer.hpp"
#include "../io/io.hpp"
#include "transform.hpp"
#include "../object/camera.hpp"

Renderer::Renderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum renderMode) :
    Component("Renderer"),
    model(model),
    material(material),
    renderMode(renderMode),
    receiveShadow(true),
    displayTexture(true)
{}

Renderer::Renderer(const Renderer& renderer) :
    Component(renderer),
    model(renderer.model->clone2()),
    material(renderer.material->clone()),
    renderMode(renderer.renderMode),
    receiveShadow(renderer.receiveShadow),
    displayTexture(renderer.displayTexture)
{}

std::shared_ptr<Renderer> Renderer::makeRenderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum renderMode) {
    std::shared_ptr<Renderer> renderer(new Renderer(model, material, renderMode));

    return renderer;
}

std::shared_ptr<Renderer> pepng::makeRenderer(std::shared_ptr<Model> model, std::shared_ptr<Material> material, GLenum renderMode) {
    return Renderer::makeRenderer(model, material, renderMode);
}

Renderer* Renderer::cloneImplementation() {
    return new Renderer(*this);
}

void Renderer::render(std::shared_ptr<WithComponents> parent, GLuint shaderProgram) {
    if(!this->model->isInit) {
        this->model->delayedInit();
    }

    if(this->model->vao == -1 || !this->isActive) {
        return;
    }

    glUseProgram(shaderProgram);
    
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, this->material->texture->getIndex());

    std::shared_ptr<Transform> transform;

    try {
        transform = parent->getComponent<Transform>();
    } catch(...) {
        std::stringstream ss;

        ss << *parent << " has no transform." << std::endl;

        throw std::runtime_error(ss.str());
    }
    
    GLuint uWorld = glGetUniformLocation(shaderProgram, "u_world");

    if (uWorld >= 0) {
        auto worldMatrix = transform->parentMatrix
            * glm::translate(glm::mat4(1.0f), this->model->offset)
            * transform->getWorldMatrix()
            * glm::translate(glm::mat4(1.0f), -this->model->offset);

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
            this->receiveShadow
        );
    }

    GLuint uDisplayTexture = glGetUniformLocation(shaderProgram, "u_display_texture");

    if(uDisplayTexture >= 0) {
        glUniform1f(
            uDisplayTexture,
            this->displayTexture
        );
    }

    glBindVertexArray(this->model->vao);

    if(this->model->hasElementArray) {
        glDrawElements(
            this->renderMode,
            this->model->count,
            GL_UNSIGNED_INT,
            0
        );
    } else {
        glDrawArrays(
            this->renderMode,
            0,
            this->model->count
        );
    }
}

void Renderer::render(std::shared_ptr<WithComponents> parent) {
    auto shaderProgram = this->material->getShaderProgram();

    glUseProgram(shaderProgram);

    if(Camera::currentCamera == nullptr) {
        throw std::runtime_error("No current camera set.");
    }

    Camera::currentCamera->render(shaderProgram);

    /**
     * TODO: This is a hacky way to select the current light.
     */
    if(Light::lights.size() > 0) {
        Light::lights.at(0)->render(shaderProgram);
    }

    this->render(parent, shaderProgram);
}

void Renderer::imgui() {
    Component::imgui();

    ImGui::LabelText("Name", this->model->name.c_str());

    std::stringstream ss;

    ss << this->model->count;

    ImGui::LabelText("Index Count", ss.str().c_str());

    const char* items[] = { "Lines", "Points", "Triangles" };
    static int item_current_idx = -1;

    if (item_current_idx == -1) {
        switch(this->renderMode) {
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
                        this->renderMode = GL_TRIANGLES;
                        break;
                    case 1:
                        this->renderMode = GL_POINTS;
                        break;
                    case 0:
                        this->renderMode = GL_LINES;
                        break;
                }
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Checkbox("Texture", &this->displayTexture);
    ImGui::Checkbox("Shadow", &this->receiveShadow);
}