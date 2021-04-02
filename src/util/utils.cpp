#include "utils.hpp"

std::vector<std::string> utils::split(const std::string& line, const std::string& delim) {
    std::vector<std::string> result;

    size_t found = line.find(delim);
    size_t startIndex = 0;

    while (found != std::string::npos) {
        std::string temp(line.begin() + startIndex, line.begin() + found);
        result.push_back(temp);
        startIndex = found + delim.size();
        found = line.find(delim, startIndex);
    }

    if (startIndex != line.size()) {
        result.push_back(std::string(line.begin() + startIndex, line.end()));
    }

    return result;
}

std::vector<int> utils::split_int(const std::string& line, const std::string& delim) {
    auto splits = split(line, delim);

    std::vector<int> ints;

    for(std::string word : splits) {
        ints.push_back(std::stoi(word));
    }

    return ints;
}

std::vector<float> utils::split_float(const std::string& line, const std::string& delim) {
    auto splits = split(line, delim);

    std::vector<float> floats;

    for(std::string word : splits) {
        floats.push_back(std::stof(word));
    }

    return floats;
}

std::filesystem::path pepng::get_folder_path(std::filesystem::path folderName) {
    auto finalPath = std::filesystem::current_path();

    int depth = 0;

    while (!std::filesystem::exists(finalPath / folderName)) {
        if(depth++ > 10) {
            throw std::runtime_error("Unable to load directory: " + folderName.string());
        }

        finalPath = finalPath.parent_path();
    }

    return finalPath / folderName;
}