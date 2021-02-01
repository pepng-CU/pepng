#pragma once

#include <vector>
#include <string>

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
}