#ifndef MESH_H
#define MESH_H

namespace Visual {
class MeshData;
class Mesh {
    typedef unsigned int GLuint;
    typedef int GLsizei;

  public:
    Mesh(Mesh &&rhc);
    Mesh(const Mesh &rhc) = delete;
    Mesh(const MeshData &data);
    Mesh(MeshData &&data);

    ~Mesh();

    void bindAndDraw() const;
    void bind() const;
    void draw() const;

    static Mesh *Quad() {
        QuadCreator *q = nullptr;
        static Mesh quad(q);
        return &quad;
    }

  private:
    void setMeshData(const MeshData &data);
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;
    GLsizei m_IndexCount = 0;
    Mesh(class QuadCreator *); // avoiding meshData class in header for quad creation
};
} // namespace Visual
#endif // MESH_H
