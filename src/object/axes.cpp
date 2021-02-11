#include "axes.hpp"
#include "../component/transform.hpp"
#include "../gl/buffer.hpp"
#include "../gl/model.hpp"

std::shared_ptr<Object> pepng::makeAxes(std::shared_ptr<Transform> transform, GLuint shaderProgram) {
    auto axes = pepng::makeObject("Axes");

    axes->attachComponent(transform);

    axes->attachComponent(pepng::makeRenderer(
            pepng::makeModel()
                ->attachBuffer(
                    pepng::makeBuffer<glm::vec3>(
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
                ->attachBuffer(
                    pepng::makeBuffer<glm::vec4>(
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
                ->attachBuffer(
                    pepng::makeBuffer<int>(
                        std::vector {
                            0, 1, 
                            2, 3, 
                            4, 5,
                        }, 
                        GL_ELEMENT_ARRAY_BUFFER
                    )
                )
                ->setElementArray(true)
                ->setCount(6)
                ->setName("Axes"), 
            shaderProgram, 
            -1, 
            GL_LINES
        )
    );

    return axes;
}