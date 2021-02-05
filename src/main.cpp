/**
 * COMP371 Final Project
 * 
 * Thank you for the template creator: Nicolas Bergeron on 20/06/2019
 * 
 * @author Alexandre Lavoie (40129457), Antoine Poulin (#), Rhys Rosenberg (#), Arman (#)
 */

#define GLEW_STATIC 1

// External includes
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <filesystem>
#include <stdlib.h>
#include <time.h>

// Internal includes
#include "shader.hpp"
#include "model.hpp"
#include "utils.hpp"
#include "camera.hpp"
#include "transform.hpp"
#include "object.hpp"

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam ) {
    // fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    //         ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
    //             type, severity, message );
}

int main(int argc, char *argv[]) {
    srand (time(NULL));

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(1024, 768, "Comp371 - Final Project", NULL, NULL);

    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Binds to refresh rate.
    glfwSwapInterval(1);

    /**
     * Debugging message handler.
     */
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    // Find the shaders directory
    auto shaderpath = utils::getPath("shaders");

    /**
     * Shader binding
     */
    std::string vertexSource = readShader(shaderpath / "vertex.glsl");
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);

    std::string fragmentSource = readShader(shaderpath / "fragment.glsl");
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    ShaderBuilder sb;

    sb << vertexShader << fragmentShader;

    GLuint shaderProgram = sb.finish();

    /**
     * Model creation.
     */
    // Find model path
    auto modelpath = utils::getPath("models");

    std::vector<std::shared_ptr<Object>> objects {
        Object::fromOBJ(modelpath / "scene.obj", shaderProgram, Transform {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(1.0f)
        })
    };

    std::vector<std::shared_ptr<Camera>> cameras {
        std::make_shared<Camera>(
            Transform {
                glm::vec3(0.0f, 2.0f, 10.0f),
                glm::vec3(0.0f),
                glm::vec3(1.0f)
            },
            Viewport {
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1024.0f, 768.0f)
            },
            glm::perspective(glm::radians(60.0f), 1024.0f / 768.0f, 0.01f, 1000.0f)
        ),
        std::make_shared<Camera>(
            Transform {
                glm::vec3(0.0f),
                glm::vec3(0.0f),
                glm::vec3(1.0f)
            },
            Viewport {
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1024.0f / 4.0f, 768.0f / 4.0f)
            },
            glm::perspective(glm::radians(60.0f), 1024.0f / 768.0f, 0.01f, 1000.0f)
        )
    };

    glm::vec2 clickPosition = glm::vec2();

    // Sets background color.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enables Z buffer.
    glEnable(GL_DEPTH_TEST);

    // Culling
    glEnable(GL_CULL_FACE);

    /**
     * Loop variables.
     */
    // Selects which object to act upon. 0 is reserved for world, 1-5 for objects.
    int selectedObject = 0;

    GLenum renderMode = GL_TRIANGLES;

    // Program loop.
    while (!glfwWindowShouldClose(window)) {
        // Clears depth + color buffer.
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        /**
         * Rendering.
         */
        // Render each camera.
        for(auto camera : cameras) {
            // Binds the viewport.
            if(camera->viewport.render()) {
                // Render each object.
                for(auto object : objects) {
                    // Bind object render to camera.
                    object->render(camera, renderMode);
                }
            }
        }

        /**
         * Transform hierarchy input.
         */
        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
            selectedObject = 0;
        } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            selectedObject = 1;
        } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            selectedObject = 2;
        } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            selectedObject = 3;
        } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            selectedObject = 4;
        } else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            selectedObject = 5;
        }

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            renderMode = GL_TRIANGLES;
        }

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            renderMode = GL_LINES;
        }

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            renderMode = GL_POINTS;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            std::shared_ptr<Object> object;

            if (selectedObject == 0) {
                object = objects.at(0);
                
            } else {
                object = objects.at(0)->children.at(selectedObject - 1);
            }

            object->transform.deltaRotate(1.0f, 0.0f);
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            std::shared_ptr<Object> object;
            
            if (selectedObject == 0) {
                object = objects.at(0);
                
            } else {
                object = objects.at(0)->children.at(selectedObject - 1);
            }

            object->transform.deltaRotate(-1.0f, 0.0f);
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            std::shared_ptr<Object> object;
            
            if (selectedObject == 0) {
                object = objects.at(0);
                
            } else {
                object = objects.at(0)->children.at(selectedObject - 1);
            }

            object->transform.deltaRotate(0.0f, -1.0f);
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            std::shared_ptr<Object> object;
            
            if (selectedObject == 0) {
                object = objects.at(0);
                
            } else {
                object = objects.at(0)->children.at(selectedObject - 1);
            }

            object->transform.deltaRotate(0.0f, 1.0f);
        }

        if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) {
            std::shared_ptr<Object> object;
            
            if (selectedObject == 0) {
                object = objects.at(0);
                
            } else {
                object = objects.at(0)->children.at(selectedObject - 1);
            }

            object->transform.position = glm::vec3(0.0f);
            object->transform.rotation = glm::quat(glm::vec3(0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            std::shared_ptr<Object> object;
            
            if (selectedObject == 0) {
                object = objects.at(0);
            } else {
                object = objects.at(0)->children.at(selectedObject - 1);
            }

            object->transform.position += object->transform.getForward() / 10.0f;
        } 

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            std::shared_ptr<Object> object;
            
            if (selectedObject == 0) {
                object = objects.at(0);
            } else {
                object = objects.at(0)->children.at(selectedObject - 1);
            }

            object->transform.position -= object->transform.getForward() / 10.0f;
        } 

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            std::shared_ptr<Object> object;
            
            if (selectedObject == 0) {
                object = objects.at(0);
            } else {
                object = objects.at(0)->children.at(selectedObject - 1);
            }

            object->transform.position -= object->transform.getRight() / 10.0f;
        } 

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            std::shared_ptr<Object> object;
            
            if (selectedObject == 0) {
                object = objects.at(0);
            } else {
                object = objects.at(0)->children.at(selectedObject - 1);
            }

            object->transform.position += object->transform.getRight() / 10.0f;
        } 

        /**
         * FPS Input.
         */
        double mouseX, mouseY;

        glfwGetCursorPos(window, &mouseX, &mouseY);

        bool leftClicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        bool middleClicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
        bool rightClicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

        for(auto camera : cameras) {
            glm::vec3 cameraForward = camera->transform.getForward();
            glm::vec3 cameraRight = camera->transform.getRight();
            glm::vec3 cameraUp = camera->transform.getUp();

            if (leftClicked || rightClicked || middleClicked) {
                if (clickPosition.x == 0 && clickPosition.y == 0) {
                    clickPosition = glm::vec2((float) mouseX, (float) mouseY);
                }
            } else {
                clickPosition = glm::vec2();
            }

            float mouseDx = std::clamp(((float) mouseX - clickPosition.x) / 40.0f, -3.0f, 3.0f);
            float mouseDy = std::clamp(((float) mouseY - clickPosition.y) / 40.0f, -3.0f, 3.0f);

            if (leftClicked) {
                camera->transform.position += cameraForward * mouseDy / 10.0f;
            }

            if (middleClicked) {
                camera->transform.position += cameraUp * mouseDy / 10.0f + cameraRight * mouseDx / 10.0f;
            }

            if (rightClicked) {
                camera->transform.deltaRotate(mouseDx, mouseDy);
            }
        }

        /**
         * GLFW events.
         */
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    glfwTerminate();

    return 0;
}