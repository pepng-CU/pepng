#include "grid.hpp"
#include "../gl/buffer.hpp"

Grid::Grid(std::shared_ptr<Transform> transform, GLuint shaderProgram, int count, glm::vec4 color) : Object("Grid", transform) {
    if (count < 2) {
        throw std::runtime_error("A grid needs at least 2 lines for edges.");
    }

    float spacing = 1.0f / (float) (count - 1);

    std::vector<glm::vec3> verticies;
    std::vector<glm::vec4> colors;
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

    this->attach(std::make_shared<Renderer>(
        Model::makeModel()
            ->attach(
                Buffer<glm::vec3>::makeBuffer(
                    verticies,
                    GL_ARRAY_BUFFER,
                    0,
                    3
                )
            )
            ->attach(
                Buffer<glm::vec4>::makeBuffer(
                    colors,
                    GL_ARRAY_BUFFER,
                    1,
                    4
                )
            )
            ->attach(
                Buffer<int>::makeBuffer(
                    indicies,
                    GL_ELEMENT_ARRAY_BUFFER
                )
            )
            ->setCount(indicies.size())
            ->setElementArray(true)
            ->setName("Grid"),
        shaderProgram,
        -1,
        GL_LINES
        )
    );
}