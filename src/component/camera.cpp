#include "camera.hpp"

Camera::Camera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection) :
    Component("Camera"),
    viewport(viewport),
    projection(projection),
    __parent(nullptr)
{}

Camera::Camera(const Camera& camera) :
    Component(camera),
    viewport(camera.viewport->clone()),
    projection(camera.projection->clone()),
    __parent(camera.__parent)
{}

std::shared_ptr<Camera> Camera::current_camera = nullptr;
std::vector<std::shared_ptr<Camera>> Camera::cameras;

std::shared_ptr<Camera> Camera::make_camera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection) {
    std::shared_ptr<Camera> camera(new Camera(viewport, projection));

    Camera::cameras.push_back(camera);

    return camera;
}

std::shared_ptr<Camera> pepng::make_camera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection) {
    return Camera::make_camera(viewport, projection);
}

Camera* Camera::clone_implementation() {
    return new Camera(*this);
}

void Camera::init(std::shared_ptr<WithComponents> parent) {
    this->__parent = parent;
}

void Camera::render(GLuint shaderProgram) {
    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "u_projection"),
        1,
        GL_FALSE,
        glm::value_ptr(this->projection->matrix())
    );

    // TODO: Should we throw if there is no parent?
    if(this->__parent == nullptr) return;

    auto transform = this->__parent->try_get_component<Transform>();

    if(transform == nullptr) {
        std::stringstream ss;

        ss << *this->__parent << " is being rendered without a transform.";

        throw std::runtime_error(ss.str());
    }

    glUniform3fv(
        glGetUniformLocation(shaderProgram, "u_camera_pos"),
        1,
        glm::value_ptr(transform->position)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "u_view"),
        1,
        GL_FALSE,
        glm::value_ptr(transform->view_matrix())
    );
}

void Camera::imgui() {
    Component::imgui();

    this->viewport->imgui();
    this->projection->imgui();
}

Viewport::Viewport(glm::vec2 position, glm::vec2 scale) : 
    position(position), 
    scale(scale), 
    __is_active(true) 
{}

Viewport::Viewport(const Viewport& viewport) : 
    position(viewport.position), 
    scale(viewport.scale),
    __is_active(viewport.__is_active) 
{}

std::shared_ptr<Viewport> Viewport::make_viewport(glm::vec2 position, glm::vec2 scale) {
    std::shared_ptr<Viewport> viewport(new Viewport(position, scale));

    return viewport;
}

std::shared_ptr<Viewport> pepng::make_viewport(glm::vec2 position, glm::vec2 scale) {
    return Viewport::make_viewport(position, scale);
}

std::shared_ptr<Viewport> Viewport::clone() {
    std::shared_ptr<Viewport> viewport(new Viewport(*this));

    return viewport;
}

bool Viewport::render(glm::vec2 windowDimension) {
    if(!this->__is_active) {
        return false;
    }

    glViewport(
        this->position.x * windowDimension.x, 
        this->position.y * windowDimension.y, 
        this->scale.x * windowDimension.x, 
        this->scale.y * windowDimension.y
    );

    return true;
}

void Viewport::imgui() {
    ImGui::InputFloat2("Position", glm::value_ptr(this->position));
    ImGui::InputFloat2("Scale", glm::value_ptr(this->scale));
}

Projection::Projection(float aspect) : _aspect(aspect) {}
Projection::Projection(const Projection& projection) : _aspect(projection._aspect) {}

Perspective::Perspective(float fovy, float aspect, float near, float far) : 
    Projection(aspect), 
    __fovy(fovy),
    __near(near), 
    __far(far) 
{}

Perspective::Perspective(const Perspective& perspective) : 
    Projection(perspective),
    __fovy(perspective.__fovy),
    __near(perspective.__near),
    __far(perspective.__far)
{}

std::shared_ptr<Perspective> Perspective::make_perspective(float fovy, float aspect, float near, float far) {
    std::shared_ptr<Perspective> perspective(new Perspective(fovy, aspect, near, far));

    return perspective;
}

std::shared_ptr<Perspective> pepng::make_perspective(float fovy, float aspect, float near, float far) {
    return Perspective::make_perspective(fovy, aspect, near, far);
}

std::shared_ptr<Projection> Perspective::clone() {
    return this->clone1();
}

std::shared_ptr<Perspective> Perspective::clone1() {
    std::shared_ptr<Perspective> perspective(new Perspective(*this));

    return perspective;
}

glm::mat4 Perspective::matrix() {
    return glm::perspective(this->__fovy, this->_aspect, this->__near, this->__far);
}

void Perspective::imgui() {
    ImGui::InputFloat("Fovy", &this->__fovy);
    ImGui::InputFloat("Near", &this->__near);
    ImGui::InputFloat("Far", &this->__far);
}