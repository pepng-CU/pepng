#include "camera.hpp"

Camera::Camera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection) :
    Component("Camera"),
    viewport(viewport),
    projection(projection),
    parent(nullptr)
{}

Camera::Camera(const Camera& camera) :
    Component(camera),
    viewport(camera.viewport->clone()),
    projection(camera.projection->clone()),
    parent(camera.parent)
{}

std::shared_ptr<Camera> Camera::currentCamera = nullptr;
std::vector<std::shared_ptr<Camera>> Camera::cameras;

std::shared_ptr<Camera> Camera::makeCamera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection) {
    std::shared_ptr<Camera> camera(new Camera(viewport, projection));

    Camera::cameras.push_back(camera);

    return camera;
}

std::shared_ptr<Camera> pepng::makeCamera(std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection) {
    return Camera::makeCamera(viewport, projection);
}

Camera* Camera::cloneImplementation() {
    return new Camera(*this);
}

void Camera::init(std::shared_ptr<WithComponents> parent) {
    this->parent = parent;
}

void Camera::render(GLuint shaderProgram) {
    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "u_projection"),
        1,
        GL_FALSE,
        glm::value_ptr(this->projection->getMatrix())
    );

    // TODO: Should we throw if there is no parent?
    if(parent == nullptr) {
        return;
    }

    auto transform = parent->getComponent<Transform>();

    if(transform == nullptr) {
        std::stringstream ss;

        ss << *this->parent << " is being rendered without a transform.";

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
        glm::value_ptr(transform->getViewMatrix())
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
    isActive(true) 
{}

Viewport::Viewport(const Viewport& viewport) : 
    position(viewport.position), 
    scale(viewport.scale),
    isActive(viewport.isActive) 
{}

std::shared_ptr<Viewport> Viewport::makeViewport(glm::vec2 position, glm::vec2 scale) {
    std::shared_ptr<Viewport> viewport(new Viewport(position, scale));

    return viewport;
}

std::shared_ptr<Viewport> pepng::makeViewport(glm::vec2 position, glm::vec2 scale) {
    return Viewport::makeViewport(position, scale);
}

std::shared_ptr<Viewport> Viewport::clone() {
    std::shared_ptr<Viewport> viewport(new Viewport(*this));

    return viewport;
}

bool Viewport::render(glm::vec2 windowDimension) {
    if(!this->isActive) {
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

Projection::Projection(float aspect) : aspect(aspect) {}
Projection::Projection(const Projection& projection) : aspect(projection.aspect) {}

void Projection::setAspect(float aspect) {
    this->aspect = aspect;
}

Perspective::Perspective(float fovy, float aspect, float near, float far) : 
    Projection(aspect), 
    fovy(fovy),
    near(near), 
    far(far) 
{}

Perspective::Perspective(const Perspective& perspective) : 
    Projection(perspective),
    fovy(perspective.fovy),
    near(perspective.near),
    far(perspective.far)
{}

std::shared_ptr<Perspective> Perspective::makePerspective(float fovy, float aspect, float near, float far) {
    std::shared_ptr<Perspective> perspective(new Perspective(fovy, aspect, near, far));

    return perspective;
}

std::shared_ptr<Perspective> pepng::makePerspective(float fovy, float aspect, float near, float far) {
    return Perspective::makePerspective(fovy, aspect, near, far);
}

std::shared_ptr<Projection> Perspective::clone() {
    return this->clone1();
}

std::shared_ptr<Perspective> Perspective::clone1() {
    std::shared_ptr<Perspective> perspective(new Perspective(*this));

    return perspective;
}

glm::mat4 Perspective::getMatrix() {
    return glm::perspective(this->fovy, this->aspect, this->near, this->far);
}

void Perspective::imgui() {
    ImGui::InputFloat("Fovy", &this->fovy);
    ImGui::InputFloat("Near", &this->near);
    ImGui::InputFloat("Far", &this->far);
}