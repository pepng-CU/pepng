#pragma once

#include <filesystem>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <map>
#include <functional>

#ifndef EMSCRIPTEN
#include <thread>
#include <future>
#endif

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

namespace pepng::extra {
    /**
     * Load from thread.
     */
    template <typename T, typename... Args>
    void load_file_thread(std::filesystem::path path, std::function<void(std::shared_ptr<T>)> function, Args... args);

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
        std::shared_ptr<Transform> transform
    );

    /**
     * Loads all textures (with their tag) from COLLADA file.
     */
    std::map<std::string, std::shared_ptr<Texture>> collada_load_textures(
        tinyxml2::XMLElement* libraryImages, 
        std::filesystem::path path
    );

    /**
     * Loads all effects (with their tag) from COLLADA file.
     */
    std::map<std::string, std::shared_ptr<Texture>> collada_load_effects(
        tinyxml2::XMLElement* libraryEffects, 
        std::map<std::string, std::shared_ptr<Texture>>& textures
    );

    /**
     * Loads all materials (with their tag) from COLLADA file.
     */
    std::map<std::string, std::shared_ptr<Material>> collada_load_materials(
        tinyxml2::XMLElement* libraryMaterials, 
        std::map<std::string, std::shared_ptr<Texture>>& effects
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
        std::map<std::string, std::shared_ptr<Material>>& materials
    );

    /**
     * Loads all scenes of objects (with their tag) from COLLADA file.
     */
    std::map<std::string, std::shared_ptr<Object>> collada_load_scenes(
        tinyxml2::XMLElement* libraryScenes, 
        std::map<std::string, std::shared_ptr<Model>>& geometries, 
        std::map<std::string, std::shared_ptr<Camera>>& cameras,
        std::map<std::string, std::shared_ptr<Material>>& materials
    );

    /**
     * Loads a complete COLLADA file (calling the callbase `function` whenever an Object is loaded).
     */
    void collada_load(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Object>)> function, 
        std::shared_ptr<Transform> transform
    );

    /**
     * Thread loader for Model.
     */
    template <>
    inline void load_file_thread(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Model>)> function
    ) {
        if(path.extension() == ".obj") {
            obj_load_model(path, function);
        } else {
            std::stringstream ss;

            ss << "Cannot load " << path.extension().string() << " model type.";

            std::cout << ss.str() << std::endl;

            throw std::runtime_error(ss.str());
        }
    }

    /**
     * Thread loader for Object.
     */
    template <>
    inline void load_file_thread(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<Object>)> function, 
        std::shared_ptr<Transform> transform
    ) {
        if(path.extension() == ".obj") {
            obj_load(path, function, transform);
        } else if(path.extension() == ".dae") {
            collada_load(path, function, transform);
        } else {
            std::stringstream ss;

            ss << "Unknown file extension: " << path.extension() << std::endl;

            std::cout << ss.str() << std::endl;

            throw std::runtime_error(ss.str());
        }
    }

    /**
     * Generic load sync class.
     */
    template <typename T, typename... Args>
    void load_file_sync(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<T>)> function, 
        Args... args
    ) {
        if(!std::filesystem::exists(path)) {
            std::stringstream ss;

            ss << "Could not find file: " << path.string() << std::endl;

            std::cout << ss.str() << std::endl;
            
            throw std::runtime_error(ss.str());
        }

        load_file_thread<T, Args...>(path, function, args...);
    }
}

namespace pepng {
    /**
     * Generic load class.
     */
    template <typename T, typename... Args>
    void load_file(
        std::filesystem::path path, 
        std::function<void(std::shared_ptr<T>)> function, 
        Args... args
    ) {
        #ifdef EMSCRIPTEN
            pepng::extra::load_file_sync<T, Args...>(path, function, args...);
        #else
            if(!std::filesystem::exists(path)) {
                std::stringstream ss;

                ss << "Could not find file: " << path.string() << std::endl;

                std::cout << ss.str() << std::endl;
                
                throw std::runtime_error(ss.str());
            }

            std::thread thread(pepng::extra::load_file_thread<T, Args...>, path, function, args...);

            thread.detach();
        #endif
    }

    /**
     * Sets object shader for future loads.
     */
    void load_set_object_shader(GLuint shader);

    /**
     * Sets shadow shader for future loads.
     */
    void load_set_shadow_shader(GLuint shader);
}  