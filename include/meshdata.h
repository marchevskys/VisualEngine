#ifndef MESHDATA_H
#define MESHDATA_H
#define DEFAULT_MESH_TYPE MeshData::Type::V

#include <array>
#include <vector>

typedef unsigned int uint;

namespace Visual {
class MeshData {
    friend class Mesh;
    friend struct MeshPrimitives;

  public:
    enum class Type { UNDEF,
                      V,
                      VN,
                      VT,
                      VTN,
                      VTNB }; /*vertex, texture, normal, binormal*/

    enum class VertexAttribute : unsigned char { Normal,
                                                 Color,
                                                 TexCoord,
                                                 Binormal
    } vertexAttribute;

    MeshData() {}
    MeshData(MeshData &&other);
    MeshData(const MeshData &other);
    void scale(float k);
    std::vector<float> makeSingleArray() const;

    template <class T>
    const std::vector<T> getVertices();

  private:
    std::vector<uint> m_indices;

    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<float> m_uvs;
    MeshData::Type m_type = DEFAULT_MESH_TYPE;
};

struct MeshPrimitives {
    static MeshData sphere(uint resolution = 4, MeshData::Type type = DEFAULT_MESH_TYPE);
    static MeshData cube(float scale_x = 1.f, float scale_y = 1.f, float scale_z = 1.f, MeshData::Type type = DEFAULT_MESH_TYPE);
    static MeshData icosahedron(MeshData::Type type = DEFAULT_MESH_TYPE);
};
} // namespace Visual
#endif // MESHDATA_H
