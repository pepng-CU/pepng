#include "texture.hpp"
#include "../component/transform.hpp"
#include "../gl/buffer.hpp"
#include "../gl/model.hpp"

std::shared_ptr<Object> pepng::makeTextureObj(std::shared_ptr<Transform> transform, GLuint shaderProgram) {
    auto axes = pepng::makeObject("Texture");

    axes->attachComponent(transform);

    auto material = pepng::makeMaterial(shaderProgram, pepng::makeTexture());

    axes->attachComponent(pepng::makeRenderer(
            pepng::makeModel()
                ->attachBuffer(
                    pepng::makeBuffer<glm::vec2>(
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
                ->attachBuffer(
                    pepng::makeBuffer<int>(
                        std::vector {
                            0, 1, 2,
                            1, 3, 2
                        }, 
                        GL_ELEMENT_ARRAY_BUFFER
                    )
                )
                ->setElementArray(true)
                ->setCount(6)
                ->setName("Texture"), 
            material,
            GL_TRIANGLES
        )
    );

    return axes;
}