#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <filesystem>

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
    std::vector<int> splitInt(const std::string& line, const std::string& delim = " ");

        /**
     * Uses word split and convert sections into float.
     * @param line
     * @param delim
     */
    std::vector<float> splitFloat(const std::string& line, const std::string& delim = " ");
}

namespace pepng {
    /**
     * Searches filesystem to find folder.
     */
    std::filesystem::path getFolderPath(std::filesystem::path folderName);
}