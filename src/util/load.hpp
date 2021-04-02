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
#include <tinyxml2.h>

#include "utils.hpp"
#include "../component/camera.hpp"
#include "../gl/model.hpp"
#include "../component/transform.hpp"
#include "../object/object.hpp"
#include "../object/camera.hpp"

namespace pepng {
    /**
     * Load from thread.
     */
    template <typename T, typename... Args>
    void load_model_file_thread(std::filesystem::path path, std::function<void(std::shared_ptr<T>)> function, Args... args);

    /**
     * Loads model geometry from OBJ file.
     * 
     * @deprecated Use obj_load instead.
     */
    void obj_load_model(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Model>)> function
    );

    /**
     * Loads model geometry and generates object for OBJ file.
     */
    void obj_load(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Object>)> function, 
        GLuint shaderProgram, 
        std::shared_ptr<Transform> transform
    );

    /**
     * Loads all textures (with their tag) from COLLADA file.
     */
    std::map<std::string, std::future<std::shared_ptr<Texture>>> collada_load_textures(
        tinyxml2::XMLElement* libraryImages, 
        std::filesystem::path path
    );

    /**
     * Loads all effects (with their tag) from COLLADA file.
     */
    std::map<std::string, std::shared_ptr<Texture>> collada_load_effects(
        tinyxml2::XMLElement* libraryEffects, 
        std::map<std::string, std::future<std::shared_ptr<Texture>>>& textures
    );

    /**
     * Loads all materials (with their tag) from COLLADA file.
     */
    std::map<std::string, std::shared_ptr<Material>> collada_load_materials(
        tinyxml2::XMLElement* libraryMaterials, 
        std::map<std::string, std::shared_ptr<Texture>>& effects, 
        GLuint shaderProgram
    );

    /**
     * Loads all cameras (with their tag) from COLLADA file.
     */
    std::map<std::string, std::shared_ptr<Camera>> collada_load_cameras(
        tinyxml2::XMLElement* libraryCameras
    );

    /**
     * Loads all geometry as model (with their tag) from COLLADA file.
     */
    std::map<std::string, std::shared_ptr<Model>> collada_load_geometries(
        tinyxml2::XMLElement* libraryGeometries
    );

    /**
     * Loads all objects recursively (with their tag) from COLLADA file.
     */
    std::vector<std::shared_ptr<Object>> collada_load_objects(
        tinyxml2::XMLElement* node, 
        std::map<std::string, std::shared_ptr<Model>>& geometries, 
        std::map<std::string, std::shared_ptr<Camera>>& cameras,
        std::map<std::string, std::shared_ptr<Material>>& materials,
        GLuint shaderProgram,
        GLuint shadowShaderProgram
    );

    /**
     * Loads all scenes of objects (with their tag) from COLLADA file.
     */
    std::map<std::string, std::shared_ptr<Object>> collada_load_scenes(
        tinyxml2::XMLElement* libraryScenes, 
        std::map<std::string, std::shared_ptr<Model>>& geometries, 
        std::map<std::string, std::shared_ptr<Camera>>& cameras,
        std::map<std::string, std::shared_ptr<Material>>& materials,
        GLuint shaderProgram,
        GLuint shadowShaderProgram
    );

    /**
     * Loads a complete COLLADA file (calling the callbase `function` whenever an Object is loaded).
     */
    void collada_load(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Object>)> function, 
        GLuint shaderProgram, 
        std::shared_ptr<Transform> transform,
        GLuint shadowShaderProgram
    );

    /**
     * Thread loader for Model.
     */
    template <>
    inline void load_model_file_thread(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Model>)> function
    ) {
        if(path.extension() == ".obj") {
            pepng::obj_load_model(path, function);
        } else {
            throw std::runtime_error("Cannot load " + path.extension().string() + " model type.");
        }
    }

    /**
     * Thread loader for Object.
     */
    template <>
    inline void load_model_file_thread(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Object>)> function, 
        GLuint shaderProgram, 
        std::shared_ptr<Transform> transform,
        GLuint shadowShaderProgram
    ) {
        if(path.extension() == ".obj") {
            obj_load(path, function, shaderProgram, transform);
        } else if(path.extension() == ".dae") {
            collada_load(path, function, shaderProgram, transform, shadowShaderProgram);
        }
    }

    /**
     * Generic load class.
     */
    template <typename T, typename... Args>
    void load_model_file(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<T>)> function, 
        Args... args
    ) {
        if(!std::filesystem::exists(path)) {
            throw std::runtime_error("Could not find file: " + path.string());
        }

        std::thread thread(load_model_file_thread<T, Args...>, path, function, args...);

        thread.detach();
    }

    /**
     * Generic load sync class.
     */
    template <typename T, typename... Args>
    void load_model_file_sync(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<T>)> function, 
        Args... args
    ) {
        if(!std::filesystem::exists(path)) {
            throw std::runtime_error("Could not find file: " + path.string());
        }

        std::thread thread(load_model_file_thread<T, Args...>, path, function, args...);

        thread.join();
    }
}  