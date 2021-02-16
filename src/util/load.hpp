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
#include <future>

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

    void loadModelOBJ(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Model>)> function
    );

    void loadObjectOBJ(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Object>)> function, 
        GLuint shaderProgram, 
        std::shared_ptr<Transform> transform
    );

    std::map<std::string, std::future<std::shared_ptr<Texture>>> loadTexturesDAE(
        tinyxml2::XMLElement* libraryImages, 
        std::filesystem::path path
    );

    std::map<std::string, std::shared_ptr<Texture>> loadEffectDAE(
        tinyxml2::XMLElement* libraryEffects, 
        std::map<std::string, std::future<std::shared_ptr<Texture>>>& textures
    );

    std::map<std::string, std::shared_ptr<Material>> loadMaterialsDAE(
        tinyxml2::XMLElement* libraryMaterials, 
        std::map<std::string, std::shared_ptr<Texture>>& effects, 
        GLuint shaderProgram
    );

    std::map<std::string, std::shared_ptr<Camera>> loadCamerasDAE(
        tinyxml2::XMLElement* libraryCameras
    );

    std::map<std::string, std::shared_ptr<Model>> loadGeometriesDAE(
        tinyxml2::XMLElement* libraryGeometries
    );

    std::vector<std::shared_ptr<Object>> loadObjectsDAE(
        tinyxml2::XMLElement* node, 
        std::map<std::string, std::shared_ptr<Model>>& geometries, 
        std::map<std::string, std::shared_ptr<Camera>>& cameras,
        std::map<std::string, std::shared_ptr<Material>>& materials,
        GLuint shaderProgram
    );

    std::map<std::string, std::shared_ptr<Object>> loadScenesDAE(
        tinyxml2::XMLElement* libraryScenes, 
        std::map<std::string, std::shared_ptr<Model>>& geometries, 
        std::map<std::string, std::shared_ptr<Camera>>& cameras,
        std::map<std::string, std::shared_ptr<Material>>& materials,
        GLuint shaderProgram
    );

    void loadObjectDAE(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Object>)> function, 
        GLuint shaderProgram, 
        std::shared_ptr<Transform> transform
    );

    template <>
    inline void loadThread(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Model>)> function
    ) {
        if(path.extension() == ".obj") {
            pepng::loadModelOBJ(path, function);
        } else {
            throw std::runtime_error("Cannot load " + path.extension().string() + " model type.");
        }
    }

    template <>
    inline void loadThread(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Object>)> function, 
        GLuint shaderProgram, 
        std::shared_ptr<Transform> transform
    ) {
        if(path.extension() == ".obj") {
            loadObjectOBJ(path, function, shaderProgram, transform);
        } else if(path.extension() == ".dae") {
            loadObjectDAE(path, function, shaderProgram, transform);
        }
    }

    template <typename T, typename... Args>
    void load(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<T>)> function, 
        Args... args
    ) {
        if(!std::filesystem::exists(path)) {
            throw std::runtime_error("Could not find file: " + path.string());
        }

        std::thread thread(loadThread<T, Args...>, path, function, args...);

        thread.detach();
    }
}  