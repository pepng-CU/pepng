#include "model.hpp"

Model::Model(int count, GLuint vao) : count(count), vao(vao) {}

void Model::render(GLuint program, GLenum mode) {
    glBindVertexArray(vao);
    
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

    bufferFromVector(vertexArray, GL_ARRAY_BUFFER, 0);
    bufferFromVector(textureArray, GL_ARRAY_BUFFER, 1);
    
    bufferFromVector(faceArray, GL_ELEMENT_ARRAY_BUFFER);

    return std::make_shared<Model>(
        faceArray.size(),
        vao
    );
}

std::vector<std::shared_ptr<Model>> Model::fromOBJ(std::filesystem::path filepath) {
    std::ifstream in(filepath);

    if(!in.is_open()) {
        throw "Unable to open obj file.";
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