#include "model.hpp"
#include "../object/objects.hpp"

Model::Model() : 
    __count(-1), 
    __vao(-1), 
    __offset(glm::vec3(0.0f, 0.0f, 0.0f)), 
    __has_element_array(false), 
    __name("Model")
{}

Model::Model(const Model& model) : 
    DelayedInit(model),
    __count(model.__count),
    __vao(model.__vao),
    __offset(model.__offset),
    __has_element_array(model.__has_element_array),
    __name(model.__name)
{}

std::shared_ptr<Model> Model::make_model() {
    std::shared_ptr<Model> model(new Model());

    return model;
}

std::shared_ptr<Model> pepng::make_model() {
    return Model::make_model();
}

Model* Model::clone_implementation() {
    return new Model(*this);
}

void Model::delayed_init() {
    if(this->_is_init) {
        return;
    }

    this->_is_init = true;

    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    this->__vao = vao;

    for(auto child : this->_delayed_children) {
        child->delayed_init();
    }
}

std::shared_ptr<Model> Model::calculate_offset(std::vector<glm::vec3> vertexArray, std::vector<unsigned int> faceArray) {
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

    this->__offset = offset;

    return shared_from_this();
}