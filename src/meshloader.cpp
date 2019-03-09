#include "meshloader.h"

#include "logger.h"
#include "mesh.h"
#include "meshdata.h"
#include "model.h"
#include "texture.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
namespace Visual {

MeshData processMesh(aiMesh *mesh, const aiScene *scene) {
    // data to fill
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        vertices.push_back({mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z});

    if (mesh->mNormals)
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            normals.push_back({mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z});

    if (mesh->mTextureCoords[0])
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            uvs.push_back({mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
    else
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            uvs.push_back({0.0f, 0.0f});

    if (mesh->mTangents)
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            tangents.push_back({mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z});

    if (mesh->mBitangents)
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            bitangents.push_back({mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z});

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    MeshData data;
    data.m_indices = std::move(indices);
    data.m_vertices = std::move(vertices);
    data.m_normals = std::move(normals);
    data.m_uvs = std::move(uvs);
    return data;
}

void processNode(aiNode *node, const aiScene *scene, std::vector<MeshData> &meshes) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, meshes);
    }
}

std::vector<MeshData> Visual::MeshLoader::load(const char *charPath) {

    std::string path(std::string(_MODEL_PATH) + std::string(charPath));
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        DLOG("ERROR::ASSIMP:: ", importer.GetErrorString());
        THROW();
    }
    // retrieve the directory path of the filepath
    std::string directory = path.substr(0, path.find_last_of('/'));

    std::vector<MeshData> meshdatas;
    processNode(scene->mRootNode, scene, meshdatas);
    return meshdatas;
}
} // namespace Visual
