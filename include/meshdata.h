// graphics-agnostic class. It contains mesh data. Positions, normals, texcoords, indices';
#ifndef MESHDATA_H
#define MESHDATA_H
#define DEFAULT_MESH_TYPE MeshData::Type::VTN

#include <glm/glm.hpp>

#include <array>
#include <vector>

typedef unsigned int uint;

namespace Visual {
class MeshData {
    friend class Mesh;
    friend struct MeshDataPrimitive;

  public:
    MeshData() {}
    MeshData(MeshData &&other);
    MeshData(const MeshData &other);
    void scale(float k);

    template <class T>
    const std::vector<T> getVertices();

    //private:
    std::vector<uint> m_indices;
    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<float> m_uvs;
};

struct MeshDataPrimitive {
    static MeshData sphere(uint resolution = 4);
    static MeshData cube(float scale_x = 1.f, float scale_y = 1.f, float scale_z = 1.f);
    static MeshData icosahedron();
    static MeshData plane(float scale);
};
} // namespace Visual
#endif // MESHDATA_H
