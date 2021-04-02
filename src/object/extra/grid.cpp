#include "grid.hpp"

#include "../../component/transform.hpp"
#include "../../gl/buffer.hpp"
#include "../../gl/model.hpp"

std::shared_ptr<Object> pepng::make_grid(std::shared_ptr<Transform> transform, GLuint shaderProgram, int count, glm::vec4 color) {
    auto grid = pepng::make_object("Grid");

    grid->attach_component(transform);

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

    auto material = pepng::make_material(shaderProgram, pepng::make_texture());

    grid->attach_component(pepng::make_renderer(
            pepng::make_model()
                ->attach_buffer(
                    pepng::make_buffer<glm::vec3>(
                        verticies,
                        GL_ARRAY_BUFFER,
                        0,
                        3
                    )
                )
                ->attach_buffer(
                    pepng::make_buffer<glm::vec4>(
                        colors,
                        GL_ARRAY_BUFFER,
                        1,
                        4
                    )
                )
                ->attach_buffer(
                    pepng::make_buffer<int>(
                        indicies,
                        GL_ELEMENT_ARRAY_BUFFER
                    )
                )
                ->set_count(indicies.size())
                ->set_element_array(true)
                ->set_name("Grid"),
            material,
            GL_LINES
        )
    );

    return grid;
}