#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.hpp"
#include "../gl/model.hpp"
#include "../component/transform.hpp"
#include "../object/object.hpp"

namespace pepng {
    template <typename T, typename... Args>
    void loadThread(std::filesystem::path path, std::function<void(std::shared_ptr<T>)> function, Args... args);

    static void loadModelOBJ(std::filesystem::path path, std::function<void(std::shared_ptr<Model>)> function) {
        std::ifstream in(path);

        if(!in.is_open()) {
            throw std::runtime_error("Unable to open obj file.");
        }

        std::string name = "";
        std::vector<glm::vec3> verticies;
        std::vector<glm::vec2> textures;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> vertexIndex;
        std::vector<unsigned int> textureIndex;
        std::vector<unsigned int> normalIndex;

        std::string line;

        while (true) {
            bool hasLine = !in.eof();

            if (hasLine) {
                std::getline(in, line);
            }

            std::stringstream ss(line);
            std::string target;
            
            ss >> target;

            if(((target == "o" || target == "v") && vertexIndex.size() != 0) || !hasLine) {
                std::vector<glm::vec3> mapVertex;
                for(auto index : vertexIndex) {
                    mapVertex.push_back(verticies.at(index));
                }

                std::vector<glm::vec2> mapTexture;
                for(auto index : textureIndex) {
                    mapTexture.push_back(textures.at(index));
                }

                std::vector<glm::vec3> mapNormal;
                for(auto index : normalIndex) {
                    mapNormal.push_back(normals.at(index));
                }

                function(
                    Model::makeModel()
                        ->setName(name)
                        ->setCount(mapVertex.size())
                        ->calculateOffset(verticies, vertexIndex)
                        ->attach(pepng::makeBuffer<glm::vec3>(mapVertex, GL_ARRAY_BUFFER, 0, 3))
                        ->attach(pepng::makeBuffer<glm::vec2>(mapTexture, GL_ARRAY_BUFFER, 2, 2))
                        ->attach(pepng::makeBuffer<glm::vec3>(mapNormal, GL_ARRAY_BUFFER, 1, 3))
                );

                vertexIndex.clear();
                textureIndex.clear();
                normalIndex.clear();
            }

            if (!hasLine) {
                break;
            }

            if (target == "o") {
                ss >> name;
            } else if (target == "v") {
                float x, y, z;

                ss >> x >> y >> z;

                verticies.push_back(glm::vec3(x, y, z));
            } else if (target == "vt") {
                float u, v;

                ss >> u >> v;

                textures.push_back(glm::vec2(u, 1.0f - v));
            } else if (target == "vn") {
                float x, y, z;

                ss >> x >> y >> z;

                normals.push_back(glm::vec3(x, y, z));
            } else if (target == "f") {
                std::string faceIndex[3];

                ss >> faceIndex[0] >> faceIndex[1] >> faceIndex[2];

                for(auto f : faceIndex) {
                    auto fa = utils::splitInt(f, "/");

                    vertexIndex.push_back(fa[0] - 1);
                    textureIndex.push_back(fa[1] - 1);
                    normalIndex.push_back(fa[2] - 1);
                }
            }
        }

        in.close();
    }

    template <>
    void loadThread(std::filesystem::path path, std::function<void(std::shared_ptr<Model>)> function) {
        if(path.extension() == ".obj") {
            pepng::loadModelOBJ(path, function);
        } else {
            throw std::runtime_error("Cannot load " + path.extension().string() + " model type.");
        }
    }

    template <>
    void loadThread(std::filesystem::path path, std::function<void(std::shared_ptr<Object>)> function, GLuint shaderProgram, std::shared_ptr<Transform> transform) {
        std::shared_ptr<Object> object = pepng::makeObject(path.filename());

        object->attachComponent(pepng::copyTransform(transform));

        loadThread(
            path,
            std::function([object, shaderProgram](std::shared_ptr<Model> model) mutable {
                auto child = pepng::makeObject(model->getName());

                child
                    ->attachComponent(pepng::makeTransform())
                    ->attachComponent(pepng::makeRenderer(model, shaderProgram, -1, GL_TRIANGLES));

                object->children.push_back(child);
            })
        );

        function(object);
    }

    template <typename T, typename... Args>
    void load(std::filesystem::path path, std::function<void(std::shared_ptr<T>)> function, Args... args) {
        std::thread thread(loadThread<T, Args...>, path, function, args...);

        thread.detach();
    }
}  