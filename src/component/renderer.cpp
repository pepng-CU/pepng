#include "renderer.hpp"
#include "../io/io.hpp"
#include "transform.hpp"
#include "../object/camera.hpp"

Renderer::Renderer(std::shared_ptr<Model> model, GLuint shaderProgram, GLuint texture, GLenum renderMode) :
    Component("Renderer"),
    model(model),
    shaderProgram(shaderProgram),
    texture(texture),
    renderMode(renderMode)
{}

std::shared_ptr<Renderer> Renderer::makeRenderer(std::shared_ptr<Model> model, GLuint shaderProgram, GLuint texture, GLenum renderMode) {
    std::shared_ptr<Renderer> renderer(new Renderer(model, shaderProgram, texture, renderMode));

    return renderer;
}

std::shared_ptr<Renderer> pepng::makeRenderer(std::shared_ptr<Model> model, GLuint shaderProgram, GLuint texture, GLenum renderMode) {
    return Renderer::makeRenderer(model, shaderProgram, texture, renderMode);
}

void Renderer::update(std::shared_ptr<WithComponents> parent) {
    if(!this->model->isInit) {
        this->model->delayedInit();
    }

    if(this->model->vao == -1 || !this->isActive) {
        return;
    }

    auto transform = parent->getComponent<Transform>();

    glUseProgram(this->shaderProgram);

    if (this->texture >= 0) {
        glBindTexture(GL_TEXTURE_2D, this->texture);
    }

    Camera::currentCamera->render(this->shaderProgram);

    GLuint uWorld = glGetUniformLocation(this->shaderProgram, "u_world");

    if (uWorld >= 0) {
        glUniformMatrix4fv(
            uWorld,
            1,
            GL_FALSE,
            glm::value_ptr(transform->parentMatrix
                * glm::translate(glm::mat4(1.0f), this->model->offset)
                * transform->getWorldMatrix()
                * glm::translate(glm::mat4(1.0f), -this->model->offset)
            )
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

    glBindTexture(GL_TEXTURE_2D, 1);
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
}