#include "mesh.h"
#include "logger.h"
#include "meshdata.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
namespace Visual {

void fillArrays(const MeshData &data, std::vector<GLfloat> &vertexArray, std::vector<GLuint> &indexArray,
                std::vector<Mesh::LODIndices> &lodIndices, size_t &indexOffset, size_t &vertexOffset,
                Mesh::OBB &bbox) {

    auto vertexCount = data.m_vertices.size();
    auto indexCount = data.m_indices.size();
    for (size_t i = 0; i < vertexCount; ++i) {
        const auto &v = data.m_vertices[i];
        const auto &n = data.m_normals[i];
        const auto &t = data.m_uvs[i];
        bbox.coverPoint(v);
        vertexArray.insert(vertexArray.end(), {v.x, v.y, v.z});
        vertexArray.insert(vertexArray.end(), {n.x, n.y, n.z});
        vertexArray.insert(vertexArray.end(), {t.x, t.y});
    }

    for (size_t i = 0; i < indexCount; ++i) // push every LOD one after another
        indexArray.push_back(data.m_indices[i] + vertexOffset);

    lodIndices.push_back({indexOffset, indexCount});

    indexOffset += indexCount;
    vertexOffset += vertexCount;
};

void pushMeshOnGPU(GLuint &VAO, GLuint &VBO, GLuint &EBO, std::vector<GLfloat> &vertexArray, std::vector<GLuint> &indexArray) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexArray.size() * sizeof(GLfloat)),
                 vertexArray.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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

Mesh::Mesh(const MeshData &data) {
    std::vector<GLfloat> vertexArray;
    std::vector<GLuint> indexArray;
    size_t indexOffset = 0, vertexOffset = 0;
    fillArrays(data, vertexArray, indexArray, m_lodIndices, indexOffset, vertexOffset, m_obb);
    m_obb.process();
    pushMeshOnGPU(m_VAO, m_VBO, m_EBO, vertexArray, indexArray);
};

Mesh::Mesh(const std::vector<MeshData> &meshDatas) {
    std::vector<GLfloat> vertexArray;
    std::vector<GLuint> indexArray;
    size_t indexOffset = 0, vertexOffset = 0;
    for (const auto &data : meshDatas)
        fillArrays(data, vertexArray, indexArray, m_lodIndices, indexOffset, vertexOffset, m_obb);
    m_obb.process();
    pushMeshOnGPU(m_VAO, m_VBO, m_EBO, vertexArray, indexArray);
}

Mesh::Mesh(const std::initializer_list<MeshData> &meshDatas) {
    std::vector<GLfloat> vertexArray;
    std::vector<GLuint> indexArray;
    size_t indexOffset = 0, vertexOffset = 0;
    for (const auto &data : meshDatas)
        fillArrays(data, vertexArray, indexArray, m_lodIndices, indexOffset, vertexOffset, m_obb);
    m_obb.process();
    pushMeshOnGPU(m_VAO, m_VBO, m_EBO, vertexArray, indexArray);
}

void Mesh::bind() const {
    glBindVertexArray(m_VAO);
}

void Mesh::draw(size_t lodLevel) const {
    assert(lodLevel < m_lodIndices.size());
    glDrawElements(GL_TRIANGLES, m_lodIndices[lodLevel].m_count,
                   GL_UNSIGNED_INT, reinterpret_cast<void *>(m_lodIndices[lodLevel].m_offset * sizeof(GLuint)));
}
void Mesh::bindAndDraw(size_t lodLevel) const {
    bind(); // bindAndDraw consists of bind and draw :)
    draw(lodLevel);
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
    m_obb = rhc.m_obb;
    m_lodIndices = std::move(rhc.m_lodIndices);
    rhc.m_VAO = 0;
}

const Mesh &MeshPrimitive::quad() {
    static Mesh qu(MeshDataPrimitive::plane(1.0f));
    return qu;
}

const std::shared_ptr<Mesh> MeshPrimitive::cube() {

    return std::make_shared<Mesh>(MeshDataPrimitive::cube());
}

const std::shared_ptr<Mesh> MeshPrimitive::lodSphere() {
    return std::shared_ptr<Mesh>(new Mesh{MeshDataPrimitive::sphere(40),
                                          MeshDataPrimitive::sphere(20),
                                          MeshDataPrimitive::sphere(10),
                                          MeshDataPrimitive::sphere(5),
                                          MeshDataPrimitive::sphere(2)});
}

} // namespace Visual
