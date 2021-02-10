#include "axes.hpp"

Axes::Axes(std::shared_ptr<Transform> transform, GLuint shaderProgram) : Object("Axes", transform) {
    this->attach(std::make_shared<Renderer>(
        Model::makeModel()
            ->attachBuffer(
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
            ->attachBuffer(
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
            ->attachBuffer(
                std::vector {
                    0, 1, 
                    2, 3, 
                    4, 5,
                }, 
                GL_ELEMENT_ARRAY_BUFFER,
                true
            )
            ->setName("Axes"), 
        shaderProgram, 
        -1, 
        GL_LINES
    ));
}