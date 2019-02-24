#include "mesh.h"
#include "logger.h"
#include "meshdata.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
namespace Visual {

void makeSingleArray(const MeshData &data, std::vector<float> &vertexArray, std::vector<uint> indexArray) {
    if (data.m_vertices.size() != data.m_normals.size() || data.m_vertices.size() != data.m_uvs.size()) {
        DLOGN(data.m_vertices.size(), data.m_normals.size(), data.m_uvs.size());
        THROW("BAD VTN");
    }

    for (size_t i = 0; i < data.m_vertices.size(); ++i) {
        auto &v = data.m_vertices[i];
        auto &n = data.m_normals[i];
        auto &t = data.m_uvs[i];
        vertexArray.insert(vertexArray.end(), {v.x, v.y, v.z});
        vertexArray.insert(vertexArray.end(), {n.x, n.y, n.z});
        vertexArray.insert(vertexArray.end(), {t.x, t.y});
    }
}

Mesh::Mesh(const MeshData &data) {
    setMeshData(data);
}

Mesh::Mesh(MeshData &&data) {
    MeshData mdata(std::move(data));
    setMeshData(mdata);
}

void Mesh::setMeshData(const MeshData &mData) {
    std::vector<float> vertexArray;
    //makeSingleArray(mData, vertexArray);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexArray.size() * sizeof(GLfloat)),
                 vertexArray.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(mData.m_indices.size() * sizeof(GLuint)),
                 mData.m_indices.data(), GL_STATIC_DRAW);
    lodIndices.push_back({0, mData.m_indices.size()});

    ////////////////////////////////////////////////////////
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    ////////////////////////////////////////////////////////

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    DLOG("Mesh created");
}

Mesh::Mesh(const std::vector<MeshData> &meshDatas) {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);

    size_t indexOffset = 0;
    size_t vertexOffset = 0;

    std::vector<GLfloat> vertexArray;
    std::vector<GLuint> indexArray;

    for (const auto &data : meshDatas) {
        auto vertexCount = data.m_vertices.size();
        auto indexCount = data.m_indices.size();

        for (size_t i = 0; i < vertexCount; ++i) {
            auto &v = data.m_vertices[i];
            auto &n = data.m_normals[i];
            auto &t = data.m_uvs[i];
            vertexArray.insert(vertexArray.end(), {v.x, v.y, v.z});
            vertexArray.insert(vertexArray.end(), {n.x, n.y, n.z});
            vertexArray.insert(vertexArray.end(), {t.x, t.y});
        }

        for (size_t i = 0; i < indexCount; ++i) {
            indexArray.push_back(data.m_indices[i] + vertexOffset);
        }
        lodIndices.push_back({indexOffset, indexCount}); // indexoffset/ indexcount
        indexOffset += indexCount;
        vertexOffset += vertexCount;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexArray.size() * sizeof(GLfloat)),
                 vertexArray.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indexArray.size() * sizeof(GLuint)),
                 indexArray.data(), GL_STATIC_DRAW);

    ////////////////////////////////////////////////////////
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    ////////////////////////////////////////////////////////

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    DLOG("Mesh created");
}

void Mesh::bind() const {
    glBindVertexArray(m_VAO);
}

void Mesh::draw(size_t lodLevel) const {
    assert(lodLevel < lodIndices.size());
    glDrawElements(GL_TRIANGLES, lodIndices[lodLevel].m_count,
                   GL_UNSIGNED_INT, reinterpret_cast<void *>(lodIndices[lodLevel].m_offset * sizeof(GLuint)));
}
void Mesh::bindAndDraw() const {
    bind();
    draw(); // :)
}

Mesh::~Mesh() {
    if (m_VAO) {
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
        glDeleteVertexArrays(1, &m_VAO);
        DLOG("Mesh destroyed");
    }
}

Mesh::Mesh(Mesh &&rhc) {
    m_VAO = rhc.m_VAO;
    m_VBO = rhc.m_VBO;
    m_EBO = rhc.m_EBO;
    lodIndices = rhc.lodIndices;
    rhc.m_VAO = 0;
};

const Mesh &MeshPrimitive::quad() {
    static Mesh qu(MeshDataPrimitive::plane(1.0f));
    return qu;
}

const Mesh &MeshPrimitive::sphere() {
    Mesh::AttributeParams params;
    params.insert({Mesh::Attribute::Vertex});
    params.insert({Mesh::Attribute::Normal});
    params.insert({Mesh::Attribute::TexCoord});
    std::vector<MeshData> meshDataLodPack{
        MeshDataPrimitive::sphere(40),
        MeshDataPrimitive::sphere(20),
        MeshDataPrimitive::sphere(10),
        MeshDataPrimitive::sphere(5),
        MeshDataPrimitive::sphere(2),
    };
    static Mesh sphereLodMesh(meshDataLodPack);
    return sphereLodMesh;
}

} // namespace Visual
