#ifndef MESH_H
#define MESH_H
#include <set>
#include <vector>
namespace Visual {
class MeshData;
class Mesh {
    typedef unsigned int GLuint;
    typedef int GLsizei;

  public:
    Mesh(Mesh &&rhc);
    Mesh(const Mesh &rhc) = delete;
    Mesh(const MeshData &data);
    Mesh(const std::vector<MeshData> &data);
    Mesh(MeshData &&data);
    ~Mesh();

    enum class Attribute { Vertex,
                           Normal,
                           TexCoord,
                           Binormal,
                           Color };

    void bindAndDraw() const;
    void bind() const;
    void draw(size_t lodLevel = 0) const;

    typedef std::set<Attribute> AttributeParams;

  private:
    struct OBB {
        float p1[3]{0};
        float p3[3]{0};
    } m_obb;

    struct LODIndices {
        size_t m_offset = 0;
        size_t m_count = 0;
    };

    void setMeshData(const MeshData &data);

    typedef std::vector<LODIndices> LodIndexMap;
    LodIndexMap lodIndices;
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;
};

class MeshPrimitive {
  public:
    static const Mesh &quad();
    static const Mesh &sphere();
};
} // namespace Visual
#endif // MESH_H
