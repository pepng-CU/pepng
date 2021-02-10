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
    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::shared_ptr<Model> model(new Model());

    model->vao = vao;

    return model;
}

std::shared_ptr<Model> Model::setCount(unsigned int count) {
    this->count = count;

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

std::vector<std::shared_ptr<Model>> Model::fromOBJ(std::filesystem::path filepath) {
    std::ifstream in(filepath);

    if(!in.is_open()) {
        throw std::runtime_error("Unable to open obj file.");
    }

    std::vector<std::shared_ptr<Model>> models;

    std::string name = "";
    std::vector<glm::vec3> verticies;
    std::vector<glm::vec2> textures;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> vertexIndex;
    std::vector<unsigned int> textureIndex;
    std::vector<unsigned int> normalIndex;

    std::string line;

    while (true) {
        bool hasLine = !in.eof();

        if (hasLine) {
            std::getline(in, line);
        }

        std::stringstream ss(line);
        std::string target;
        
        ss >> target;

        if(((target == "o" || target == "v") && vertexIndex.size() != 0) || !hasLine) {
            std::vector<glm::vec3> mapVertex;
            for(auto index : vertexIndex) {
                mapVertex.push_back(verticies.at(index));
            }

            std::vector<glm::vec2> mapTexture;
            for(auto index : textureIndex) {
                mapTexture.push_back(textures.at(index));
            }

            std::vector<glm::vec3> mapNormal;
            for(auto index : normalIndex) {
                mapNormal.push_back(normals.at(index));
            }

            models.push_back(
                Model::makeModel()
                    ->setName(name)
                    ->attachBuffer(mapVertex, GL_ARRAY_BUFFER, 0, 3)
                    ->attachBuffer(mapTexture, GL_ARRAY_BUFFER, 2, 2)
                    ->attachBuffer(mapNormal, GL_ARRAY_BUFFER, 1, 3)
                    ->setCount(mapVertex.size())
                    ->calculateOffset(verticies, vertexIndex)
            );

            vertexIndex.clear();
            textureIndex.clear();
            normalIndex.clear();
        }

        if (!hasLine) {
            break;
        }

        if (target == "o") {
            ss >> name;
        } else if (target == "v") {
            float x, y, z;

            ss >> x >> y >> z;

            verticies.push_back(glm::vec3(x, y, z));
        } else if (target == "vt") {
            float u, v;

            ss >> u >> v;

            textures.push_back(glm::vec2(u, 1.0f - v));
        } else if (target == "vn") {
            float x, y, z;

            ss >> x >> y >> z;

            normals.push_back(glm::vec3(x, y, z));
        } else if (target == "f") {
            std::string faceIndex[3];

            ss >> faceIndex[0] >> faceIndex[1] >> faceIndex[2];

            for(auto f : faceIndex) {
                auto fa = utils::splitInt(f, "/");

                vertexIndex.push_back(fa[0] - 1);
                textureIndex.push_back(fa[1] - 1);
                normalIndex.push_back(fa[2] - 1);
            }
        }
    }

    in.close();

    return models;
}