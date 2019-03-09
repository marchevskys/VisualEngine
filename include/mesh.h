#ifndef MESH_H
#define MESH_H
#include <set>
#include <vector>

#include <glm/glm.hpp>
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
    int getLodCount() const { return lodIndices.size(); };

    typedef std::set<Attribute> AttributeParams;

  private:
    struct OBB {
        glm::vec3 min;
        glm::vec3 max;
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
};
} // namespace Visual
#endif // MESH_H
