#include "model.hpp"

Model::Model() : Model(-1, -1, glm::vec3(0.0f)) {}

Model::Model(const Model& model) : Model(model.count, model.vao, glm::vec3(model.offset)) {}

Model::Model(int count, GLuint vao) : Model(count, vao, glm::vec3(0.0f)) {}

Model::Model(int count, GLuint vao, glm::vec3 offset) : count(count), vao(vao), offset(offset) {}

void Model::render(GLuint program, GLenum mode) {
    if (this->vao == -1) {
        return;
    }

    glBindVertexArray(this->vao);
    
    glDrawElements(
        mode,
        this->count,
        GL_UNSIGNED_INT,
        0
    );
}

std::shared_ptr<Model> Model::fromVectors(std::vector<glm::vec3> vertexArray, std::vector<glm::vec2> textureArray, std::vector<unsigned int> faceArray) {
    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    utils::bufferFromVector(vertexArray, GL_ARRAY_BUFFER, 0);
    utils::bufferFromVector(textureArray, GL_ARRAY_BUFFER, 1);

    utils::bufferFromVector(faceArray, GL_ELEMENT_ARRAY_BUFFER);

    // Calculates the pivot since OBJ don't have any... Assumes average position.
    int count = 0;
    glm::vec3 offset = glm::vec3(0.0f);
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

    return std::make_shared<Model>(
        faceArray.size(),
        vao,
        offset
    );
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
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textures;
    std::vector<unsigned int> faces;

    std::string line;

    while (std::getline(in, line)) {
        std::stringstream ss(line);

        std::string target;

        ss >> target;

        if (target == "o") {
            ss >> name;
        } else if (target == "v") {
            if (faces.size() != 0) {
                models.push_back(Model::fromVectors(verticies, textures, faces));

                faces.clear();
            }

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
            std::string f1, f2, f3;

            ss >> f1 >> f2 >> f3;

            std::vector<int> fa1, fa2, fa3;

            fa1 = utils::splitInt(f1, "/");
            fa2 = utils::splitInt(f2, "/");
            fa3 = utils::splitInt(f3, "/");

            faces.push_back(fa1[0] - 1);
            faces.push_back(fa2[0] - 1);
            faces.push_back(fa3[0] - 1);
        }
    }

    in.close();

    if (faces.size() != 0) {
        models.push_back(Model::fromVectors(verticies, textures, faces));
    }

    return models;
}