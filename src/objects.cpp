#include "objects.hpp"

Axes::Axes() : Object() {}

Axes::Axes(Transform transform, GLuint shaderProgram) : Object(transform, shaderProgram) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    utils::bufferFromVector(std::vector { 
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f), 
        glm::vec3(0.0f, 0.0f, 0.0f), 
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f), 
        glm::vec3(0.0f, 0.0f, 1.0f)
    }, GL_ARRAY_BUFFER, 0);

    utils::bufferFromVector(std::vector { 
        glm::vec3(1.0f, 0.0f, 0.0f), 
        glm::vec3(1.0f, 0.0f, 0.0f), 
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    }, GL_ARRAY_BUFFER, 1);

    utils::bufferFromVector(std::vector {
        0, 1, 
        2, 3, 
        4, 5,
    }, GL_ELEMENT_ARRAY_BUFFER);

    this->model = std::make_shared<Model>(6, vao);
}

void Axes::render(std::shared_ptr<Camera> camera, GLenum mode, glm::mat4 parentMatrix) {
    glLineWidth(5.0f);

    Object::render(camera, mode, parentMatrix);

    glLineWidth(1.0f);
}

Grid::Grid() : Object() {}

Grid::Grid(Transform transform, GLuint shaderProgram, int count) : Object(transform, shaderProgram) {
    if (count < 2) {
        throw std::runtime_error("A grid needs at least 2 lines for edges.");
    }

    glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.0f);

    float spacing = 1.0f / (float) (count - 1);

    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::vector<glm::vec3> verticies;
    std::vector<glm::vec3> colors;
    std::vector<int> indicies;

    for(float x = 0; x < count; x++) {
        verticies.push_back(glm::vec3(x * spacing - 0.5f, 0.0f, -0.5f));
        verticies.push_back(glm::vec3(x * spacing - 0.5f, 0.0f, 0.5f));
    }

    for(float y = 0; y < count; y++) {
        verticies.push_back(glm::vec3(-0.5f, 0.0f, y * spacing - 0.5f));
        verticies.push_back(glm::vec3(0.5f, 0.0f, y * spacing - 0.5f));
    }

    for(int i = 0; i < verticies.size(); i++) {
        indicies.push_back(i);
        colors.push_back(color);
    }

    utils::bufferFromVector(verticies, GL_ARRAY_BUFFER, 0);
    utils::bufferFromVector(colors, GL_ARRAY_BUFFER, 1);
    utils::bufferFromVector(indicies, GL_ELEMENT_ARRAY_BUFFER);

    this->model = std::make_shared<Model>(indicies.size(), vao);
}