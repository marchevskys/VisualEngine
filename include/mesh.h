#ifndef MESH_H
#define MESH_H
namespace Visual{
class MeshData;
class Mesh {
    typedef unsigned int GLuint;
    typedef int GLsizei;
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;
    GLsizei m_IndexCount = 0;

  public:
    Mesh(Mesh &&rhc);
    Mesh(const MeshData &data);
    Mesh(MeshData &&data);
    void setMeshData(const MeshData &data);
    void render() const;
    ~Mesh();

    // define mesh as no-copyable
    Mesh(const Mesh &rhc) = delete;
};
}
#endif // MESH_H
