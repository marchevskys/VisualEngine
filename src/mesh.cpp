#include "mesh.h"
#include "logger.h"
#include "meshdata.h"

#include <GL/glew.h>

#include <vector>
namespace Visual {
Mesh::Mesh(const MeshData &data) {
    setMeshData(data);
}

Mesh::Mesh(MeshData &&data) {
    MeshData mdata(std::move(data));
    setMeshData(mdata);
}

void Mesh::setMeshData(const MeshData &mData) {
    auto vertexArray = mData.makeSingleArray();

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
    m_IndexCount = static_cast<GLsizei>(mData.m_indices.size());

    switch (mData.m_type) {
    case MeshData::Type::V: {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);

    } break;

    case MeshData::Type::VT: {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    } break;

    case MeshData::Type::VN: {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    } break;

    case MeshData::Type::VTN: {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    } break;

    case MeshData::Type::VTNB: {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(8 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    } break;
    default: {
        THROW("Bad mesh type");
    }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    DLOG("Mesh created");
}

void Mesh::bind() const {
    glBindVertexArray(m_VAO);
}

void Mesh::draw() const {
    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
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
    m_IndexCount = rhc.m_IndexCount;
    rhc.m_VAO = 0;
};

Mesh::Mesh(QuadCreator *) : Mesh(MeshPrimitives::plane(1.0f, MeshData::Type::VT)) {}

} // namespace Visual
