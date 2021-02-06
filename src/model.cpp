#include "model.hpp"

ModelBuilder::ModelBuilder() : model(std::make_shared<Model>()) {
    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    this->model->vao = vao;
}

ModelBuilder* ModelBuilder::setCount(unsigned int count) {
    this->model->count = count;

    return this;
}

ModelBuilder* ModelBuilder::setName(std::string name) {
    this->model->name = name;

    return this;
}

ModelBuilder* ModelBuilder::calculateOffset(std::vector<glm::vec3> vertexArray, std::vector<unsigned int> faceArray) {
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

    this->model->offset = offset;

    return this;
}

std::shared_ptr<Model> ModelBuilder::build() {
    return this->model;
}

Model::Model() : count(-1), vao(-1), offset(glm::vec3(0.0f, 0.0f, 0.0f)), hasElementArray(false) {}

Model::Model(const Model& model) : count(model.count), vao(model.vao), offset(glm::vec3(model.offset)), hasElementArray(model.hasElementArray) {}

void Model::render(GLuint program, GLenum mode) {
    if (this->vao == -1) {
        return;
    }

    glBindVertexArray(this->vao);

    if(this->hasElementArray) {
        glDrawElements(
            mode,
            this->count,
            GL_UNSIGNED_INT,
            0
        );
    } else {
        glDrawArrays(
            mode,
            0,
            this->count
        );
    }
}

glm::mat4 Model::getOffsetMatrix() {
    return glm::translate(glm::mat4(1.0f), this->offset);
}

glm::mat4 Model::getNegativeOffsetMatrix() {
    return glm::translate(glm::mat4(1.0f), -this->offset);
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

        if((target == "v" && vertexIndex.size() != 0) || !hasLine) {
            std::vector<glm::vec3> mapVertex;
            for(auto index : vertexIndex) {
                mapVertex.push_back(verticies.at(index));
            }

            std::vector<glm::vec3> mapNormal;
            for(auto index : normalIndex) {
                mapNormal.push_back(normals.at(index));
            }

            std::vector<glm::vec2> mapTexture;
            for(auto index : textureIndex) {
                mapTexture.push_back(textures.at(index));
            }

            models.push_back(
                std::make_shared<ModelBuilder>()
                    ->setName(name)
                    ->attachBuffer(mapVertex, GL_ARRAY_BUFFER, 0, 3)
                    ->attachBuffer(mapNormal, GL_ARRAY_BUFFER, 1, 3)
                    ->attachBuffer(mapTexture, GL_ARRAY_BUFFER, 2, 2)
                    ->setCount(mapVertex.size())
                    ->calculateOffset(verticies, vertexIndex)
                    ->build()
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

            textures.push_back(glm::vec2(u, v));
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

GLuint createTexture(std::filesystem::path filePath) {
    int width, height, numComponents;

    unsigned char* image = stbi_load(filePath.c_str(), &width, &height, &numComponents, STBI_rgb);

    if (image == NULL)
        throw std::runtime_error("Cannot load texture");

    GLuint texture;

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);

    return texture;
}