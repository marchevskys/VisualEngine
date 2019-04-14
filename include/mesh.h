#ifndef MESH_H
#define MESH_H
#include <initializer_list>
#include <memory>
#include <set>
#include <vector>

#include <glm/glm.hpp>
namespace Visual {
class MeshData;
typedef unsigned int GLuint;
typedef int GLsizei;
class Mesh {
  public:
    struct OBB { // object's bounding box
        OBB() : min(FLT_MAX), max(-FLT_MAX) { center = glm::vec3(0), scale = glm::vec3(1); }
        OBB(glm::vec3 _min, glm::vec3 _max) : min(_min), max(_max) { process(); }
        void coverPoint(const glm::vec3 &v) { min = glm::min(v, min), max = glm::max(v, max); }
        void process() { center = (min + max) * 0.5f, scale = (max - min) * 0.5f; }
        glm::vec3 min;
        glm::vec3 max;
        glm::vec3 center;
        glm::vec3 scale;
    };

    enum class Attribute { Vertex, // attributes of mesh
                           Normal,
                           TexCoord,
                           Binormal,
                           Color };

    typedef std::set<Attribute> AttributeParams;

    struct LODIndices {
        size_t m_offset = 0;
        size_t m_count = 0;
    };

    Mesh(Mesh &&rhc);
    Mesh(const MeshData &data);
    Mesh(const std::vector<MeshData> &data);
    Mesh(const std::initializer_list<MeshData> &data);

    Mesh(const Mesh &rhc) = delete;
    Mesh &operator=(const Mesh &rhc) = delete;

    ~Mesh();

    void bindAndDraw(size_t lodLevel = 0) const;
    void bind() const;
    void draw(size_t lodLevel = 0) const;

    size_t getLodCount() const { return m_lodIndices.size(); };
    const OBB &getOBB() const { return m_obb; }

  private:
    OBB m_obb;
    std::vector<LODIndices> m_lodIndices;
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;
};

class MeshPrimitive {
  public:
    static const Mesh &quad();
    static const std::shared_ptr<Mesh> &cube();
    static const std::shared_ptr<Mesh> &lodSphere();
};
} // namespace Visual
#endif // MESH_H
