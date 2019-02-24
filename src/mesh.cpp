#include "mesh.h"
#include "logger.h"
#include "meshdata.h"

#include <GL/glew.h>

#include <vector>
namespace Visual {

std::vector<float> makeSingleArray(const MeshData &data) {
    std::vector<float> array;

    if (data.m_vertices.size() != data.m_normals.size() || data.m_vertices.size() != data.m_uvs.size() / 2 * 3) {
        DLOGN(data.m_vertices.size(), data.m_normals.size(), data.m_uvs.size());
        THROW("BAD VTN");
    }
    array.reserve(data.m_vertices.size() + data.m_normals.size() + data.m_uvs.size());
    for (size_t i = 0, j = 0; i < data.m_vertices.size(); i += 3) {
        array.push_back(data.m_vertices[i]);
        array.push_back(data.m_vertices[i + 1]);
        array.push_back(data.m_vertices[i + 2]);
        array.push_back(data.m_normals[i]);
        array.push_back(data.m_normals[i + 1]);
        array.push_back(data.m_normals[i + 2]);
        array.push_back(data.m_uvs[j++]);
        array.push_back(data.m_uvs[j++]);
    }

    return array;
}

Mesh::Mesh(const MeshData &data) {
    setMeshData(data);
}

Mesh::Mesh(MeshData &&data) {
    MeshData mdata(std::move(data));
    setMeshData(mdata);
}

void Mesh::setMeshData(const MeshData &mData) {

    auto vertexArray = makeSingleArray(mData);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
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

void Mesh::bind() const {
    glBindVertexArray(m_VAO);
}

void Mesh::draw(int lodLevel) const {
    glDrawElements(GL_TRIANGLES, lodIndices[lodLevel].m_count,
                   GL_UNSIGNED_INT, reinterpret_cast<void *>(lodIndices[lodLevel].m_offset));
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
}

} // namespace Visual
