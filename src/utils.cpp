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

std::vector<int> utils::splitInt(const std::string& line, const std::string& delim) {
    auto splits = split(line, delim);

    std::vector<int> ints;

    for(std::string word : splits) {
        ints.push_back(std::stoi(word));
    }

    return ints;
}