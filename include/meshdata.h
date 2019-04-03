// graphics-agnostic class. It contains mesh data. Positions, normals, texcoords, indices';
#ifndef MESHDATA_H
#define MESHDATA_H
#define DEFAULT_MESH_TYPE MeshData::Type::VTN

#include <glm/glm.hpp>

#include <array>
#include <vector>
#include "mymath.h"

typedef unsigned int uint;

namespace Visual {
class MeshData {
    friend class Mesh;
    friend struct MeshDataPrimitive;

  public:
    MeshData() {}
    MeshData(MeshData &&other) noexcept;
    MeshData(const MeshData &other);
    void scale(float k);

    template <class T>
    const std::vector<T> getVertices();

    //private:
    std::vector<uint> m_indices;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uvs;
};

struct MeshDataPrimitive {
    static MeshData sphere(uint resolution = 4);
    static MeshData cube(glm::vec3 scale = glm::vec3(1.0f));
    static MeshData icosahedron();
    static MeshData plane(float scale);
};
} // namespace Visual
#endif // MESHDATA_H
