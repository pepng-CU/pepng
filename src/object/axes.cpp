#include "axes.hpp"
#include "../component/transform.hpp"
#include "../gl/buffer.hpp"
#include "../gl/model.hpp"

std::shared_ptr<Object> pepng::make_axes(std::shared_ptr<Transform> transform, GLuint shaderProgram) {
    auto axes = pepng::make_object("Axes");

    axes->attach_component(transform);

    auto material = pepng::make_material(shaderProgram, pepng::make_texture());

    axes->attach_component(pepng::make_renderer(
            pepng::make_model()
                ->attach_buffer(
                    pepng::make_buffer<glm::vec3>(
                        std::vector { 
                            glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(1.0f, 0.0f, 0.0f), 
                            glm::vec3(0.0f, 0.0f, 0.0f), 
                            glm::vec3(0.0f, 1.0f, 0.0f),
                            glm::vec3(0.0f, 0.0f, 0.0f), 
                            glm::vec3(0.0f, 0.0f, 1.0f)
                        },
                        GL_ARRAY_BUFFER, 
                        0, 
                        3
                    )
                )
                ->attach_buffer(
                    pepng::make_buffer<glm::vec4>(
                        std::vector { 
                            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 
                            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 
                            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                            glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                            glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
                        }, 
                        GL_ARRAY_BUFFER, 
                        1, 
                        4
                    )
                )
                ->attach_buffer(
                    pepng::make_buffer<int>(
                        std::vector {
                            0, 1, 
                            2, 3, 
                            4, 5,
                        }, 
                        GL_ELEMENT_ARRAY_BUFFER
                    )
                )
                ->set_element_array(true)
                ->set_count(6)
                ->set_name("Axes"), 
            material,
            GL_LINES
        )
    );

    return axes;
}