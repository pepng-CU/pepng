#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../component/transform.hpp"
#include "../object/object.hpp"

namespace pepng {
    template <typename T, typename... Args>
    void loadThread(std::filesystem::path path, std::function<void(std::shared_ptr<T>)> function, Args... args);

    template <>
    void loadThread(std::filesystem::path path, std::function<void(std::shared_ptr<Object>)> function, GLuint shaderProgram, std::shared_ptr<Transform> transform) {
        auto object = Object::fromOBJ(path, shaderProgram, transform);

        function(object);
    }

    template <typename T, typename... Args>
    void load(std::filesystem::path path, std::function<void(std::shared_ptr<T>)> function, Args... args) {
        std::thread thread(loadThread<T, Args...>, path, function, args...);

        thread.detach();
    }
}  