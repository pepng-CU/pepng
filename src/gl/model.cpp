#include "model.hpp"
#include "../object/objects.hpp"

Model::Model() : 
    count(-1), 
    vao(-1), 
    offset(glm::vec3(0.0f, 0.0f, 0.0f)), 
    hasElementArray(false), 
    name("Model")
{}

std::shared_ptr<Model> Model::makeModel() {
    std::shared_ptr<Model> model(new Model());

    return model;
}

std::shared_ptr<Model> pepng::makeModel() {
    return Model::makeModel();
}
void Model::delayedInit() {
    if(this->isInit) {
        return;
    }

    this->isInit = true;

    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    this->vao = vao;

    for(auto child : this->delayedChildren) {
        child->delayedInit();
    }
}

std::shared_ptr<Model> Model::setCount(unsigned int count) {
    this->count = count;

    return shared_from_this();
}

std::shared_ptr<Model> Model::setElementArray(bool hasElementArray) {
    this->hasElementArray = hasElementArray;

    return shared_from_this();
}

std::shared_ptr<Model> Model::setName(std::string name) {
    this->name = name;

    return shared_from_this();
}

std::string Model::getName() {
    return this->name;
}

std::shared_ptr<Model> Model::calculateOffset(std::vector<glm::vec3> vertexArray, std::vector<unsigned int> faceArray) {
    int count = 0;
    glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
    std::unordered_set<unsigned int> seenPoints;

    for(auto faceIndex : faceArray) {
        if(seenPoints.find(faceIndex) != seenPoints.end()) {
            continue;
        }

        seenPoints.insert(faceIndex);
        offset += vertexArray[faceIndex];
        count++;
    }

    if(count > 0) {
        offset /= count;
    }

    this->offset = offset;

    return shared_from_this();
}