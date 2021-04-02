#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace utils {
    /**
     * Split a string into an array of words based on a delimiter
     * @param line
     * @param delim
     */
    std::vector<std::string> split(const std::string& line, const std::string& delim = " ");

    /**
     * Uses word split and convert sections into ints.
     * @param line
     * @param delim
     */
    std::vector<int> split_int(const std::string& line, const std::string& delim = " ");

        /**
     * Uses word split and convert sections into float.
     * @param line
     * @param delim
     */
    std::vector<float> split_float(const std::string& line, const std::string& delim = " ");
}

namespace pepng {
    /**
     * Searches filesystem to find folder.
     */
    std::filesystem::path get_folder_path(std::filesystem::path folderName);
}