#include "load.hpp"

void pepng::obj_load_model(std::filesystem::path path, std::function<void(std::shared_ptr<Model>)> function) {
    std::ifstream in(path);

    if(!in.is_open()) {
        throw std::runtime_error("Unable to open obj file.");
    }

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

            function(
                Model::make_model()
                    ->set_name(name)
                    ->set_count(mapVertex.size())
                    ->calculate_offset(verticies, vertexIndex)
                    ->attach_buffer(pepng::make_buffer<glm::vec3>(mapVertex, GL_ARRAY_BUFFER, 0, 3))
                    ->attach_buffer(pepng::make_buffer<glm::vec3>(mapNormal, GL_ARRAY_BUFFER, 1, 3))
                    ->attach_buffer(pepng::make_buffer<glm::vec2>(mapTexture, GL_ARRAY_BUFFER, 2, 2))
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
                auto fa = utils::split_int(f, "/");

                vertexIndex.push_back(fa[0] - 1);
                textureIndex.push_back(fa[1] - 1);
                normalIndex.push_back(fa[2] - 1);
            }
        }
    }

    in.close();
}

void pepng::obj_load(
    std::filesystem::path path, 
    std::function<void(std::shared_ptr<Object>)> function, 
    GLuint shaderProgram, 
    std::shared_ptr<Transform> transform
) {
    std::string name = path.filename().string();
    std::shared_ptr<Object> object = pepng::make_object(name);
    
    object->attach_component(transform->clone());

    load_model_file_thread(
        path,
        std::function([object, shaderProgram](std::shared_ptr<Model> model) mutable {
            auto child = pepng::make_object(model->name());

            child
                ->attach_component(pepng::make_transform())
                ->attach_component(
                    pepng::make_renderer(
                        model, 
                        pepng::make_material(
                            shaderProgram, 
                            pepng::make_texture()
                        ), 
                        GL_TRIANGLES
                    )
                );

            object->children.push_back(child);
        })
    );

    function(object);
}

std::shared_ptr<Texture> loadTexture(std::filesystem::path path) {
    return pepng::make_texture(path);
}

std::map<std::string, std::future<std::shared_ptr<Texture>>> pepng::collada_load_textures(
    tinyxml2::XMLElement* libraryImages, 
    std::filesystem::path path
) {
    std::map<std::string, std::future<std::shared_ptr<Texture>>> textures;

    if(libraryImages == nullptr) {
        return textures;
    }

    auto texture = libraryImages->FirstChildElement("image");

    while(texture != nullptr) {
        std::string textureId = texture->FindAttribute("id")->Value();

        std::filesystem::path texturePath = texture->FirstChildElement("init_from")->GetText();

        if(texturePath.is_relative()) {
            texturePath = path.parent_path() / texturePath;
        }

        textures[textureId] = std::async(loadTexture, texturePath);

        std::cout << "Loaded texture: " << textureId << std::endl;

        texture = texture->NextSiblingElement("image");
    }

    return textures;
}

std::map<std::string, std::shared_ptr<Texture>> pepng::collada_load_effects(
    tinyxml2::XMLElement* libraryEffects, 
    std::map<std::string, std::future<std::shared_ptr<Texture>>>& textures
) {
    std::map<std::string, std::shared_ptr<Texture>> effects;

    if(libraryEffects == nullptr) {
        return effects;
    }

    auto effect = libraryEffects->FirstChildElement("effect");

    while(effect != nullptr) {
        std::string effectId = "#";
        effectId += effect->FindAttribute("id")->Value();

        auto profile = effect->FirstChildElement("profile_COMMON");

        auto newparam = profile->FirstChildElement("newparam");

        bool effectLoaded = false;

        while(newparam != nullptr) {
            auto param = newparam->FirstChildElement();

            std::string paramName = param->Name();

            if(paramName == "surface") {
                auto textureId = param->FirstChildElement("init_from")->GetText();

                if(textures.find(textureId) == textures.end()) {
                    std::stringstream ss;

                    ss << "Could not find texture " << textureId << std::endl;

                    throw std::runtime_error(ss.str());
                }

                effects[effectId] = textures[textureId].get();

                effectLoaded = true;

                break;
            }

            newparam = newparam->NextSiblingElement("newparam");
        }

        if(!effectLoaded) {
            effects[effectId] = pepng::make_texture();
        }

        std::cout << "Loaded effect: " << effectId << std::endl;

        effect = effect->NextSiblingElement("effect");
    }

    return effects;
}

std::map<std::string, std::shared_ptr<Material>> pepng::collada_load_materials(
    tinyxml2::XMLElement* libraryMaterials, 
    std::map<std::string, std::shared_ptr<Texture>>& effects, 
    GLuint shaderProgram
) {
    std::map<std::string, std::shared_ptr<Material>> materials;

    if(libraryMaterials == nullptr) {
        return materials;
    }

    auto material = libraryMaterials->FirstChildElement("material");

    while(material != nullptr) {
        std::string materialId = "#";
        materialId += material->FindAttribute("id")->Value();

        auto effect = material->FirstChildElement("instance_effect");

        std::string effectId = effect->FindAttribute("url")->Value();

        auto texture = effects[effectId];

        if (texture == nullptr) {
            throw std::runtime_error("Could not find effect " + effectId);
        }

        materials[materialId] = pepng::make_material(shaderProgram, texture);

        std::cout << "Loaded material: " << materialId << std::endl;

        material = material->NextSiblingElement("material");
    }

    return materials;
}

std::map<std::string, std::shared_ptr<Camera>> pepng::collada_load_cameras(
    tinyxml2::XMLElement* libraryCameras
) {
    std::map<std::string, std::shared_ptr<Camera>> cameras;

    if(libraryCameras == nullptr) {
        return cameras;
    }

    auto camera = libraryCameras->FirstChildElement("camera");

    while(camera != nullptr) {
        std::string cameraId = "#";
        cameraId += camera->FindAttribute("id")->Value();

        auto optics = camera->FirstChildElement("optics");

        auto techniqueCommon = optics->FirstChildElement("technique_common");

        std::shared_ptr<Projection> projection;

        if(auto perspective = techniqueCommon->FirstChildElement("perspective")) {
            auto fovy = glm::radians(std::stof(perspective->FirstChildElement("xfov")->GetText()));
            auto aspect = std::stof(perspective->FirstChildElement("aspect_ratio")->GetText());
            auto near = std::stof(perspective->FirstChildElement("znear")->GetText());
            auto far = std::stof(perspective->FirstChildElement("zfar")->GetText());

            projection = pepng::make_perspective(fovy, aspect, near, far);
        }

        auto cameraComponent = pepng::make_camera(pepng::make_viewport(glm::vec2(0.0f), glm::vec2(1.0f)), projection);

        cameras[cameraId] = cameraComponent;

        std::cout << "Loaded camera: " << cameraId << std::endl;

        camera = camera->NextSiblingElement("camera");
    }

    return cameras;
}

std::map<std::string, std::shared_ptr<Model>> pepng::collada_load_geometries(
    tinyxml2::XMLElement* libraryGeometries
) {
    std::map<std::string, std::shared_ptr<Model>> geometries;

    if(libraryGeometries == nullptr) {
        return geometries;
    }

    auto geometry = libraryGeometries->FirstChildElement("geometry");

    while(geometry != nullptr) {
        std::string geometryId = "#";
        geometryId += geometry->FindAttribute("id")->Value();

        std::string geometryName = geometry->FindAttribute("name")->Value();

        auto mesh = geometry->FirstChildElement("mesh");

        std::map<std::string, std::pair<std::vector<float>, float>> sourceArrays;

        auto source = mesh->FirstChildElement("source");

        while(source != nullptr) {
            std::string sourceId = source->FindAttribute("id")->Value();

            auto floatArrayTag = source->FirstChildElement("float_array");

            auto floatArray = utils::split_float(floatArrayTag->GetText());

            if(floatArray.size() != floatArrayTag->FindAttribute("count")->IntValue()) {
                std::stringstream ss;

                ss  << "Expect index count to be "
                    << floatArrayTag->FindAttribute("count")->IntValue()
                    << " but got "
                    << floatArray.size();

                throw std::runtime_error(ss.str());
            }

            auto size = source->FirstChildElement("technique_common")->FirstChildElement("accessor")->FindAttribute("stride")->IntValue();

            sourceArrays["#" + sourceId] = std::pair(floatArray, size);

            source = source->NextSiblingElement("source");
        }

        auto vertices = mesh->FirstChildElement("vertices");

        std::string vertexId = vertices->FindAttribute("id")->Value();

        std::string vertexSource = vertices->FirstChildElement("input")->FindAttribute("source")->Value();

        sourceArrays["#" + vertexId] = sourceArrays[vertexSource];

        auto triangles = mesh->FirstChildElement("triangles");

        auto indicies = utils::split_int(triangles->FirstChildElement("p")->GetText());

        auto model = pepng::make_model();

        model->set_count(triangles->FindAttribute("count")->IntValue() * 3);

        model->set_name(geometryName);

        auto input = triangles->FirstChildElement("input");

        while(input != nullptr) {
            auto offset = input->FindAttribute("offset")->IntValue();

            std::string sourceId = input->FindAttribute("source")->Value();

            auto [sourceArray, sourceSize] = sourceArrays[sourceId];

            std::vector<std::vector<float>> sourcePacked;

            std::vector<float> pointArray;

            for(auto point : sourceArray) {
                if(pointArray.size() == sourceSize) {
                    sourcePacked.push_back(pointArray);

                    pointArray = std::vector<float>();
                }

                pointArray.push_back(point);
            }

            if(pointArray.size() == sourceSize) {
                sourcePacked.push_back(pointArray);
            }

            std::vector<float> sourceBuffer;

            // TODO: Potentially bad assuming offset of 3?
            for(int i = offset; i < indicies.size(); i += 3) {
                for(auto point : sourcePacked.at(indicies.at(i))) {
                    sourceBuffer.push_back(point);
                }
            }

            auto buffer = pepng::make_buffer<float>(sourceBuffer, GL_ARRAY_BUFFER, offset, sourceSize);

            model->attach_buffer(buffer);

            input = input->NextSiblingElement("input");
        }

        geometries[geometryId] = model;

        std::cout << "Loaded geometry: " << geometryId << std::endl;

        geometry = geometry->NextSiblingElement("geometry");
    }

    return geometries;
}

std::shared_ptr<Object> collada_load_object_data(
    tinyxml2::XMLElement* node, 
    std::map<std::string, std::shared_ptr<Model>>& geometries, 
    std::map<std::string, std::shared_ptr<Camera>>& cameras,
    std::map<std::string, std::shared_ptr<Material>>& materials,
    GLuint shaderProgram,
    GLuint shadowShaderProgram
) {
    std::string objectName = node->FindAttribute("name")->Value();

    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotationQuat = glm::quat(glm::vec3(0.0f));
    glm::vec3 scale = glm::vec3(1.0f);

    auto translate = node->FirstChildElement("translate");

    while(translate != nullptr) {
        std::string sid = translate->FindAttribute("sid")->Value();

        if (sid == "location") {
            auto p = utils::split_float(translate->GetText());

            position = glm::vec3(p[0], p[1], p[2]);
        }

        translate = translate->NextSiblingElement("translate");
    }

    auto scaleTag = node->FirstChildElement("scale");

    while(scaleTag != nullptr) {
        std::string sid = scaleTag->FindAttribute("sid")->Value();

        if (sid == "scale") {
            auto s = utils::split_float(scaleTag->GetText());

            scale = glm::vec3(s[0], s[1], s[2]);
        }

        scaleTag = scaleTag->NextSiblingElement("scale");
    }

    auto rotate = node->FirstChildElement("rotate");

    while(rotate != nullptr) {
        auto r = utils::split_float(rotate->GetText());

        rotationQuat *= glm::quat(r[0], r[1], r[2], r[3]);

        rotate = rotate->NextSiblingElement("rotate");
    }

    auto matrix = node->FirstChildElement("matrix");

    while(matrix != nullptr) {
        std::string sid = matrix->FindAttribute("sid")->Value();

        if(sid == "transform") {
            auto transformArray = utils::split_float(matrix->GetText());

            if(transformArray.size() != 16) {
                throw std::runtime_error("Expected mat4x4 but got array of size " + transformArray.size());
            }

            auto transformMatrix = glm::mat4(0.0f);

            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 4; j++) {
                    transformMatrix[j][i] = transformArray.at(i * 4 + j);
                }
            }

            glm::vec3 skew;
            glm::vec4 perspective;

            glm::decompose(transformMatrix, scale, rotationQuat, position, skew, perspective);
        }

        matrix = matrix->NextSiblingElement("matrix");
    }

    auto rotation = glm::degrees(glm::eulerAngles(rotationQuat));

    std::shared_ptr<Object> object = nullptr;

    if(auto iCamera = node->FirstChildElement("instance_camera")) {
        rotation = glm::vec3(90.0f - rotation.x, -rotation.y, rotation.z);

        std::string cameraId = iCamera->FindAttribute("url")->Value();

        auto camera = cameras[cameraId];

        object = pepng::make_camera_object(pepng::make_camera_transform(position, rotation, scale), camera);
    } else {
        object = pepng::make_object(objectName);

        object->attach_component(pepng::make_transform(position, rotation, scale));
    }

    if(auto iLight = node->FirstChildElement("instance_light")) {
        // TODO: Import actual light values instead of default.
        object->attach_component(pepng::make_light(shadowShaderProgram, glm::vec3(1.0f)));
    }

    if(auto iGeometry = node->FirstChildElement("instance_geometry")) {
        std::string geometryId = iGeometry->FindAttribute("url")->Value();

        auto geometry = geometries[geometryId];

        if(auto bindMaterial = iGeometry->FirstChildElement("bind_material")) {
            auto instanceMaterial = bindMaterial->FirstChildElement("technique_common")->FirstChildElement("instance_material");

            std::string materialId = instanceMaterial->FindAttribute("target")->Value();

            auto material = materials[materialId];

            if (material == nullptr) {
                throw std::runtime_error("Could not find material " + materialId);
            }

            object->attach_component(pepng::make_renderer(geometry, material));
        } else {
            object->attach_component(pepng::make_renderer(geometry, pepng::make_material(shaderProgram, pepng::make_texture())));
        }
    }

    std::cout << "Loaded object: " << objectName << std::endl;

    object->children = pepng::collada_load_objects(node, geometries, cameras, materials, shaderProgram, shadowShaderProgram);

    return object;
}

std::vector<std::shared_ptr<Object>> pepng::collada_load_objects(
    tinyxml2::XMLElement* node, 
    std::map<std::string, std::shared_ptr<Model>>& geometries, 
    std::map<std::string, std::shared_ptr<Camera>>& cameras,
    std::map<std::string, std::shared_ptr<Material>>& materials,
    GLuint shaderProgram,
    GLuint shadowShaderProgram
) {
    std::vector<std::shared_ptr<Object>> objects;

    auto objNode = node->FirstChildElement("node");

    while(objNode != nullptr) {
        objects.push_back(collada_load_object_data(objNode, geometries, cameras, materials, shaderProgram, shadowShaderProgram));

        objNode = objNode->NextSiblingElement("node");
    }

    return objects;
}

std::map<std::string, std::shared_ptr<Object>> pepng::collada_load_scenes(
    tinyxml2::XMLElement* libraryScenes, 
    std::map<std::string, std::shared_ptr<Model>>& geometries, 
    std::map<std::string, std::shared_ptr<Camera>>& cameras,
    std::map<std::string, std::shared_ptr<Material>>& materials,
    GLuint shaderProgram,
    GLuint shadowShaderProgram
) {
    std::map<std::string, std::shared_ptr<Object>> scenes;

    auto scene = libraryScenes->FirstChildElement("visual_scene");

    while(scene != nullptr) {
        std::string sceneName = scene->FindAttribute("name")->Value();

        auto sceneObj = pepng::make_object(sceneName);

        sceneObj->attach_component(pepng::make_transform());

        sceneObj->children = collada_load_objects(scene, geometries, cameras, materials, shaderProgram, shadowShaderProgram);

        scenes[sceneName] = sceneObj;

        std::cout << "Loaded scene: " << sceneName << std::endl;

        scene = scene->NextSiblingElement("visual_scene");
    }

    return scenes;
}

void pepng::collada_load(
    std::filesystem::path path, 
    std::function<void(std::shared_ptr<Object>)> function, 
    GLuint shaderProgram, 
    std::shared_ptr<Transform> transform,
    GLuint shadowShaderProgram
) {
    std::cout << "Loading COLLADA: " << path << std::endl;

    tinyxml2::XMLDocument doc;

    doc.LoadFile(path.string().c_str());

    const clock_t beginTime = clock();

    auto root = doc.RootElement();

    auto futureCameras = std::async(collada_load_cameras, root->FirstChildElement("library_cameras"));

    auto futureGeometries = std::async(collada_load_geometries, root->FirstChildElement("library_geometries"));

    auto textures = collada_load_textures(root->FirstChildElement("library_images"), path);

    auto effects = collada_load_effects(root->FirstChildElement("library_effects"), textures);

    auto materials = collada_load_materials(root->FirstChildElement("library_materials"), effects, shaderProgram);

    auto cameras = futureCameras.get();

    auto geometries = futureGeometries.get();

    auto scenes = collada_load_scenes(root->FirstChildElement("library_visual_scenes"), geometries, cameras, materials, shaderProgram, shadowShaderProgram);

    for(auto scene : scenes) {
        function(scene.second);
    }

    std::cout << "Loaded COLLADA: " << path << std::endl;
}