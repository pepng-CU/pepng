#include "axes.hpp"
#include "../gl/buffer.hpp"

Axes::Axes(std::shared_ptr<Transform> transform, GLuint shaderProgram) : Object("Axes", transform) {
    this->attach(std::make_shared<Renderer>(
        Model::makeModel()
            ->attach(
                Buffer<glm::vec3>::makeBuffer(
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
            ->attach(
                Buffer<glm::vec4>::makeBuffer(
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
            ->attach(
                Buffer<int>::makeBuffer(
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
    ));
}