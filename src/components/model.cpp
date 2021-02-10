#include "model.hpp"
#include "../objects/camera.hpp"

ModelBuilder::ModelBuilder() : model(std::make_shared<Model>()) {
    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    this->model->vao = vao;
}

ModelBuilder& ModelBuilder::setCount(unsigned int count) {
    this->model->count = count;

    return *this;
}

ModelBuilder& ModelBuilder::setName(std::string name) {
    this->model->name = name;

    return *this;
}

ModelBuilder& ModelBuilder::setRenderMode(GLenum mode) {
    this->model->renderMode = mode;

    return *this;
}

ModelBuilder& ModelBuilder::setShaderProgram(GLuint shaderProgram) {
    this->model->shaderProgram = shaderProgram;

    return *this;
}

ModelBuilder& ModelBuilder::calculateOffset(std::vector<glm::vec3> vertexArray, std::vector<unsigned int> faceArray) {
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

    return *this;
}

std::shared_ptr<Model> ModelBuilder::build() {
    return this->model;
}

Model::Model() : 
    Component("Model"),
    renderMode(GL_TRIANGLES),
    count(-1), 
    vao(-1), 
    offset(glm::vec3(0.0f, 0.0f, 0.0f)), 
    hasElementArray(false), 
    name("Empty"),
    shaderProgram(-1)
{}

Model::Model(const Model& model) : 
    Component("Model"),
    renderMode(GL_TRIANGLES),
    count(model.count), 
    vao(model.vao), 
    offset(glm::vec3(model.offset)), 
    hasElementArray(model.hasElementArray),
    name(model.name),
    shaderProgram(model.shaderProgram)
{}

void Model::update(std::shared_ptr<Transform> parent) {
    if (this->vao == -1 || !this->isActive) {
        return;
    }

    glUseProgram(this->shaderProgram);

    Camera::currentCamera->render(this->shaderProgram);

    GLuint uWorld = glGetUniformLocation(shaderProgram, "u_world");

    if(uWorld >= 0) {
        glUniformMatrix4fv(
            uWorld,
            1,
            GL_FALSE,
            glm::value_ptr(parent->parentMatrix
                * glm::translate(glm::mat4(1.0f), this->offset)
                * parent->getWorldMatrix()
                * glm::translate(glm::mat4(1.0f), -this->offset)
            )
        );
    }

    glBindVertexArray(this->vao);

    if(this->hasElementArray) {
        glDrawElements(
            this->renderMode,
            this->count,
            GL_UNSIGNED_INT,
            0
        );
    } else {
        glDrawArrays(
            this->renderMode,
            0,
            this->count
        );
    }
}

std::vector<std::shared_ptr<Model>> Model::fromOBJ(std::filesystem::path filepath, GLuint shaderProgram) {
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

            std::vector<glm::vec3> mapNormal;
            for(auto index : normalIndex) {
                mapNormal.push_back(normals.at(index));
            }

            std::vector<glm::vec2> mapTexture;
            for(auto index : textureIndex) {
                mapTexture.push_back(textures.at(index));
            }

            models.push_back(
                ModelBuilder()
                    .setName(name)
                    .attachBuffer(mapVertex, GL_ARRAY_BUFFER, 0, 3)
                    .attachBuffer(mapNormal, GL_ARRAY_BUFFER, 1, 3)
                    .attachBuffer(mapTexture, GL_ARRAY_BUFFER, 2, 2)
                    .setCount(mapVertex.size())
                    .calculateOffset(verticies, vertexIndex)
                    .setShaderProgram(shaderProgram)
                    .build()
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

void Model::imgui() {
    Component::imgui();

    ImGui::LabelText("Name", this->name.c_str());
    std::stringstream ss;

    ss << this->count;

    ImGui::LabelText("Index Count", ss.str().c_str());

    const char* items[] = { "Triangles", "Points", "Lines" };
    static int item_current_idx = -1;

    if (item_current_idx == -1) {
        switch(this->renderMode) {
            case GL_TRIANGLES:
                item_current_idx = 0;
                break;
            case GL_POINTS:
                item_current_idx = 1;
                break;
            case GL_LINES:
                item_current_idx = 2;
        }
    }

    const char* combo_label = items[item_current_idx];

    if (ImGui::BeginCombo("Render", combo_label)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
            
            if (ImGui::Selectable(items[n], is_selected)) {
                item_current_idx = n;

                switch(item_current_idx) {
                    case 0:
                        this->renderMode = GL_TRIANGLES;
                        break;
                    case 1:
                        this->renderMode = GL_POINTS;
                        break;
                    case 2:
                        this->renderMode = GL_LINES;
                        break;
                }
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

GLuint createTexture(const std::filesystem::path& filePath) {
    int width, height, numComponents;

    #ifdef _MSC_VER
        const std::string& filePathString = (const std::string&) filePath.u8string();
    #else
        const std::string& filePathString = (const std::string&) filePath;
    #endif

    unsigned char* image = stbi_load(filePath.c_str(), &width, &height, &numComponents, STBI_rgb);

    if (image == NULL){
        std::cout << "Cannot load texture: " << filePath << std::endl;
        throw std::runtime_error("Cannot load texture");
    }

    GLuint texture;

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);

    return texture;
}