#include "grid.hpp"
#include "../gl/buffer.hpp"

std::shared_ptr<Object> pepng::makeGrid(std::shared_ptr<Transform> transform, GLuint shaderProgram, int count, glm::vec4 color) {
    auto grid = pepng::makeObject("Grid");

    grid->attachComponent(transform);

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

    grid->attachComponent(pepng::makeRenderer(
            pepng::makeModel()
                ->attach(
                    pepng::makeBuffer<glm::vec3>(
                        verticies,
                        GL_ARRAY_BUFFER,
                        0,
                        3
                    )
                )
                ->attach(
                    pepng::makeBuffer<glm::vec4>(
                        colors,
                        GL_ARRAY_BUFFER,
                        1,
                        4
                    )
                )
                ->attach(
                    pepng::makeBuffer<int>(
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

    return grid;
}