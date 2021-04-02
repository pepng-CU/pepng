#include "texture.hpp"

#include "../../component/transform.hpp"
#include "../../gl/buffer.hpp"
#include "../../gl/model.hpp"

std::shared_ptr<Object> pepng::make_texture_object(std::shared_ptr<Transform> transform, GLuint shaderProgram) {
    auto axes = pepng::make_object("Texture");

    axes->attach_component(transform);

    auto material = pepng::make_material(shaderProgram, pepng::make_texture());

    axes->attach_component(pepng::make_renderer(
            pepng::make_model()
                ->attach_buffer(
                    pepng::make_buffer<glm::vec2>(
                        std::vector { 
                            glm::vec2(-1.0f, -1.0f),
                            glm::vec2(1.0f, -1.0f), 
                            glm::vec2(-1.0f, 1.0f), 
                            glm::vec2(1.0f, 1.0f)
                        },
                        GL_ARRAY_BUFFER, 
                        0, 
                        2
                    )
                )
                ->attach_buffer(
                    pepng::make_buffer<int>(
                        std::vector {
                            0, 1, 2,
                            1, 3, 2
                        }, 
                        GL_ELEMENT_ARRAY_BUFFER
                    )
                )
                ->set_element_array(true)
                ->set_count(6)
                ->set_name("Texture"), 
            material,
            GL_TRIANGLES
        )
    );

    return axes;
}