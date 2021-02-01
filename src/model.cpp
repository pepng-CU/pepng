#include "model.hpp"

Model::Model(int count, GLuint vertexBuffer, GLuint faceBuffer) : count(count), vertexBuffer(vertexBuffer), faceBuffer(faceBuffer) {}

void Model::render(GLuint program) {
    GLuint vertexPosition = glGetAttribLocation(program, "a_position");
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->faceBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);

    glVertexAttribPointer(
        vertexPosition, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        0, 
        0
    );

    glEnableVertexAttribArray(vertexPosition);

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "MODEL ATTRIB ERROR: " << err << std::endl;
    }

    glDrawElements(
        GL_TRIANGLES,
        this->count,
        GL_UNSIGNED_INT,
        0
    );

    while((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "MODEL ELEMENT ERROR: " << err << std::endl;
    }
}

GLuint bufferFromFloatVector(std::vector<glm::vec3> vectors, GLenum target) {
    GLuint buffer;

    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, vectors.size() * sizeof(glm::vec3), glm::value_ptr(vectors[0]), GL_STATIC_DRAW);
    glBindBuffer(target, 0);

    return buffer;
}

GLuint bufferFromIntVector(std::vector<unsigned int> vector, GLenum target) {
    GLuint buffer;

    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, vector.size() * sizeof(unsigned int), &vector[0], GL_STATIC_DRAW);
    glBindBuffer(target, 0);

    return buffer;
}

std::shared_ptr<Model> Model::fromVectors(std::vector<glm::vec3> vertexArray, std::vector<unsigned int> faceArray) {
    auto vertexBuffer = bufferFromFloatVector(vertexArray, GL_ARRAY_BUFFER);
    auto faceBuffer = bufferFromIntVector(faceArray, GL_ELEMENT_ARRAY_BUFFER);

    return std::make_shared<Model>(
        faceArray.size(),
        vertexBuffer, 
        faceBuffer
    );
}

std::vector<std::shared_ptr<Model>> Model::fromOBJ(const char* path) {
    std::filesystem::path file(path);

    std::ifstream in(std::filesystem::current_path() / file);

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
            if (name != "") {
                models.push_back(Model::fromVectors(verticies, faces));
            }

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

    if (name != "") {
        models.push_back(Model::fromVectors(verticies, faces));
    }

    return models;
}