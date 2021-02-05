#include "model.hpp"

ModelBuilder::ModelBuilder() : model(std::make_shared<Model>()) {
    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    this->model->vao = vao;
}

ModelBuilder* ModelBuilder::setName(std::string name) {
    this->model->name = name;

    return this;
}

ModelBuilder* ModelBuilder::calculateOffset(std::vector<glm::vec3> vertexArray, std::vector<unsigned int> faceArray) {
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

    this->model->offset = offset;

    return this;
}

std::shared_ptr<Model> ModelBuilder::build() {
    return this->model;
}

Model::Model() : count(-1), vao(-1), offset(glm::vec3(0.0f)) {}

Model::Model(const Model& model) : count(model.count), vao(model.vao), offset(glm::vec3(model.offset)) {}

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

    while (true) {
        bool hasLine = !in.eof();

        if (hasLine) {
            std::getline(in, line);
        }

        std::stringstream ss(line);
        std::string target;
        
        ss >> target;

        if((target == "v" && faces.size() != 0) || !hasLine) {
            models.push_back(
                std::make_shared<ModelBuilder>()
                    ->setName(name)
                    ->attachBuffer(verticies, GL_ARRAY_BUFFER, 0, 3)
                    ->attachBuffer(normals, GL_ARRAY_BUFFER, 1, 3)
                    ->attachBuffer(textures, GL_ARRAY_BUFFER, 2, 2)
                    ->attachBuffer(faces, GL_ELEMENT_ARRAY_BUFFER, true)
                    ->calculateOffset(verticies, faces)
                    ->build()
            );

            faces.clear();
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

    return models;
}