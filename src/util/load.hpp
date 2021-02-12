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
#include <map>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <tinyxml2.h>

#include "utils.hpp"
#include "../component/camera.hpp"
#include "../gl/model.hpp"
#include "../component/transform.hpp"
#include "../object/object.hpp"
#include "../object/camera.hpp"

namespace pepng {
    template <typename T, typename... Args>
    void loadThread(std::filesystem::path path, std::function<void(std::shared_ptr<T>)> function, Args... args);

    void loadModelOBJ(std::filesystem::path path, std::function<void(std::shared_ptr<Model>)> function) {
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
                        ->attachBuffer(pepng::makeBuffer<glm::vec3>(mapVertex, GL_ARRAY_BUFFER, 0, 3))
                        ->attachBuffer(pepng::makeBuffer<glm::vec3>(mapNormal, GL_ARRAY_BUFFER, 1, 3))
                        ->attachBuffer(pepng::makeBuffer<glm::vec2>(mapTexture, GL_ARRAY_BUFFER, 2, 2))
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

                textures.push_back(glm::vec2(u, v));
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

    void loadObjectOBJ(std::filesystem::path path, std::function<void(std::shared_ptr<Object>)> function, GLuint shaderProgram, std::shared_ptr<Transform> transform) {
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

    std::map<std::string, std::shared_ptr<Camera>> loadCameraDAE(tinyxml2::XMLElement* libraryCameras) {
        std::map<std::string, std::shared_ptr<Camera>> cameras;

        auto camera = libraryCameras->FirstChildElement("camera");

        while(camera != nullptr) {
            std::string cameraId = "#";
            cameraId += camera->FindAttribute("id")->Value();

            auto optics = camera->FirstChildElement("optics");

            auto techniqueCommon = optics->FirstChildElement("technique_common");

            std::shared_ptr<Projection> projection;

            if(auto perspective = techniqueCommon->FirstChildElement("perspective")) {
                auto fovy = glm::radians(std::stof(perspective->FirstChildElement("xfov")->GetText()));
                auto aspect = std::stof(perspective->FirstChildElement("aspect_ratio")->GetText());
                auto near = std::stof(perspective->FirstChildElement("znear")->GetText());
                auto far = std::stof(perspective->FirstChildElement("zfar")->GetText());

                projection = pepng::makePerspective(fovy, aspect, near, far);
            }

            auto cameraComponent = pepng::makeCamera(pepng::makeViewport(glm::vec2(0.0f), glm::vec2(1.0f)), projection);

            cameras[cameraId] = cameraComponent;

            camera = camera->NextSiblingElement("camera");
        }

        return cameras;
    }

    std::map<std::string, std::shared_ptr<Model>> loadGeometryDAE(tinyxml2::XMLElement* libraryGeometries) {
        std::map<std::string, std::shared_ptr<Model>> geometries;

        auto geometry = libraryGeometries->FirstChildElement("geometry");

        while(geometry != nullptr) {
            std::string geometryId = "#";
            geometryId += geometry->FindAttribute("id")->Value();

            std::string geometryName = geometry->FindAttribute("name")->Value();

            auto mesh = geometry->FirstChildElement("mesh");

            std::map<std::string, std::pair<std::vector<float>, float>> sourceArrays;

            auto source = mesh->FirstChildElement("source");

            while(source != nullptr) {
                std::string sourceId = source->FindAttribute("id")->Value();

                auto floatArrayTag = source->FirstChildElement("float_array");

                auto floatArray = utils::splitFloat(floatArrayTag->GetText());

                if(floatArray.size() != floatArrayTag->FindAttribute("count")->IntValue()) {
                    throw std::runtime_error("Expect index count to be " + std::to_string(floatArrayTag->FindAttribute("count")->IntValue()) + " but got " + std::to_string(floatArray.size()));
                }

                auto size = source->FirstChildElement("technique_common")->FirstChildElement("accessor")->FindAttribute("stride")->IntValue();

                sourceArrays["#" + sourceId] = std::pair(floatArray, size);

                source = source->NextSiblingElement("source");
            }

            auto vertices = mesh->FirstChildElement("vertices");

            std::string vertexId = vertices->FindAttribute("id")->Value();

            std::string vertexSource = vertices->FirstChildElement("input")->FindAttribute("source")->Value();

            sourceArrays["#" + vertexId] = sourceArrays[vertexSource];

            auto triangles = mesh->FirstChildElement("triangles");

            auto indicies = utils::splitInt(triangles->FirstChildElement("p")->GetText());

            auto model = pepng::makeModel();

            model->setCount(triangles->FindAttribute("count")->IntValue() * 3);

            model->setName(geometryName);

            auto input = triangles->FirstChildElement("input");

            while(input != nullptr) {
                auto offset = input->FindAttribute("offset")->IntValue();

                std::string sourceId = input->FindAttribute("source")->Value();

                auto [sourceArray, sourceSize] = sourceArrays[sourceId];

                std::vector<std::vector<float>> sourcePacked;

                std::vector<float> pointArray;

                for(auto point : sourceArray) {
                    if(pointArray.size() == sourceSize) {
                        sourcePacked.push_back(pointArray);

                        pointArray = std::vector<float>();
                    }

                    pointArray.push_back(point);
                }

                if(pointArray.size() == sourceSize) {
                    sourcePacked.push_back(pointArray);
                }

                std::vector<float> sourceBuffer;

                // TODO: Potentially bad assuming offset of 3?
                for(int i = offset; i < indicies.size(); i += 3) {
                    for(auto point : sourcePacked.at(indicies.at(i))) {
                        sourceBuffer.push_back(point);
                    }
                }

                auto buffer = pepng::makeBuffer<float>(sourceBuffer, GL_ARRAY_BUFFER, offset, sourceSize);

                model->attachBuffer(buffer);

                input = input->NextSiblingElement("input");
            }

            geometries[geometryId] = model;

            geometry = geometry->NextSiblingElement("geometry");
        }

        return geometries;
    }

    std::vector<std::shared_ptr<Object>> loadObjectDAEDeep(
        tinyxml2::XMLElement* node, 
        std::map<std::string, std::shared_ptr<Model>>& geometries, 
        std::map<std::string, std::shared_ptr<Camera>>& cameras,
        GLuint shaderProgram
    ) {
        std::vector<std::shared_ptr<Object>> objects;

        auto myNode = node->FirstChildElement("node");

        while(myNode != nullptr) {
            std::string objectName = myNode->FindAttribute("name")->Value();

            glm::vec3 position = glm::vec3(0.0f);
            glm::quat rotationQuat = glm::quat(glm::vec3(0.0f));
            glm::vec3 scale = glm::vec3(1.0f);

            auto translate = myNode->FirstChildElement("translate");

            while(translate != nullptr) {
                std::string sid = translate->FindAttribute("sid")->Value();

                if (sid == "location") {
                    auto p = utils::splitFloat(translate->GetText());

                    position = glm::vec3(p[0], p[1], p[2]);
                }

                translate = translate->NextSiblingElement("translate");
            }

            auto scaleTag = myNode->FirstChildElement("scale");

            while(scaleTag != nullptr) {
                std::string sid = scaleTag->FindAttribute("sid")->Value();

                if (sid == "scale") {
                    auto s = utils::splitFloat(scaleTag->GetText());

                    scale = glm::vec3(s[0], s[1], s[2]);
                }

                scaleTag = scaleTag->NextSiblingElement("scale");
            }

            auto rotate = myNode->FirstChildElement("rotate");

            while(rotate != nullptr) {
                auto r = utils::splitFloat(rotate->GetText());

                rotationQuat *= glm::quat(r[0], r[1], r[2], r[3]);

                rotate = rotate->NextSiblingElement("rotate");
            }

            auto matrix = myNode->FirstChildElement("matrix");

            while(matrix != nullptr) {
                std::string sid = matrix->FindAttribute("sid")->Value();

                if(sid == "transform") {
                    auto transformArray = utils::splitFloat(matrix->GetText());

                    if(transformArray.size() != 16) {
                        throw std::runtime_error("Expected mat4x4 but got array of size " + transformArray.size());
                    }

                    auto transformMatrix = glm::mat4(0.0f);

                    for(int i = 0; i < 4; i++) {
                        for(int j = 0; j < 4; j++) {
                            transformMatrix[j][i] = transformArray.at(i * 4 + j);
                        }
                    }

                    glm::vec3 skew;
                    glm::vec4 perspective;

                    glm::decompose(transformMatrix, scale, rotationQuat, position, skew, perspective);
                }

                matrix = matrix->NextSiblingElement("matrix");
            }

            auto rotation = glm::degrees(glm::eulerAngles(rotationQuat));

            std::shared_ptr<Object> object = nullptr;

            if(auto iCamera = myNode->FirstChildElement("instance_camera")) {
                rotation = glm::vec3(90.0f - rotation.x, -rotation.y, rotation.z);

                std::cout << glm::to_string(rotation) << std::endl;

                std::string cameraId = iCamera->FindAttribute("url")->Value();

                auto camera = cameras[cameraId];

                object = pepng::makeCameraObj(pepng::makeCameraTransform(position, rotation, scale), camera);
            } else {
                object = pepng::makeObject(objectName);

                object->attachComponent(pepng::makeTransform(position, rotation, scale));
            }

            if(auto iGeometry = myNode->FirstChildElement("instance_geometry")) {
                std::string geometryId = iGeometry->FindAttribute("url")->Value();

                auto geometry = geometries[geometryId];

                object->attachComponent(pepng::makeRenderer(geometry, shaderProgram));
            }

            object->children = loadObjectDAEDeep(myNode, geometries, cameras, shaderProgram);

            objects.push_back(object);

            myNode = myNode->NextSiblingElement("node");
        }

        return objects;
    }

    std::map<std::string, std::shared_ptr<Object>> loadSceneDAE(
        tinyxml2::XMLElement* libraryScenes, 
        std::map<std::string, std::shared_ptr<Model>> geometries, 
        std::map<std::string, std::shared_ptr<Camera>>& cameras,
        GLuint shaderProgram
    ) {
        std::map<std::string, std::shared_ptr<Object>> scenes;

        auto scene = libraryScenes->FirstChildElement("visual_scene");

        while(scene != nullptr) {
            std::string sceneName = scene->FindAttribute("name")->Value();

            auto sceneObj = pepng::makeObject(sceneName);

            sceneObj->attachComponent(pepng::makeTransform());

            sceneObj->children = loadObjectDAEDeep(scene, geometries, cameras, shaderProgram);

            scenes[sceneName] = sceneObj;

            scene = scene->NextSiblingElement("visual_scene");
        }

        return scenes;
    }

    void loadObjectDAE(std::filesystem::path path, std::function<void(std::shared_ptr<Object>)> function, GLuint shaderProgram, std::shared_ptr<Transform> transform) {
        tinyxml2::XMLDocument doc;

        doc.LoadFile(path.c_str());

        auto root = doc.RootElement();

        auto cameras = loadCameraDAE(root->FirstChildElement("library_cameras"));

        auto geometries = loadGeometryDAE(root->FirstChildElement("library_geometries"));

        auto scenes = loadSceneDAE(root->FirstChildElement("library_visual_scenes"), geometries, cameras, shaderProgram);

        for(auto scene : scenes) {
            function(scene.second);
        }
    }

    template <>
    void loadThread(std::filesystem::path path, std::function<void(std::shared_ptr<Object>)> function, GLuint shaderProgram, std::shared_ptr<Transform> transform) {
        if(path.extension() == ".obj") {
            loadObjectOBJ(path, function, shaderProgram, transform);
        } else if(path.extension() == ".dae") {
            loadObjectDAE(path, function, shaderProgram, transform);
        }
    }

    template <typename T, typename... Args>
    void load(std::filesystem::path path, std::function<void(std::shared_ptr<T>)> function, Args... args) {
        if(!std::filesystem::exists(path)) {
            throw std::runtime_error("Could not find file: " + path.string());
        }

        std::thread thread(loadThread<T, Args...>, path, function, args...);

        thread.detach();
    }
}  