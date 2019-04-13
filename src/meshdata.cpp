#include "meshdata.h"
#include "logger.h"

#include <algorithm>
#include <glm/glm.hpp>
#if defined _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>
namespace Visual {

MeshData::MeshData(MeshData &&other) noexcept {
    m_indices = std::move(other.m_indices);
    m_vertices = std::move(other.m_vertices);
    m_normals = std::move(other.m_normals);
    m_uvs = std::move(other.m_uvs);
}

MeshData::MeshData(const MeshData &other) {
    THROW("Copied");
}

void MeshData::scale(float k) {
    for (auto &v : m_vertices)
        v *= k;
}

template <>
const std::vector<glm::vec3> MeshData::getVertices() { return m_vertices; }

MeshData MeshDataPrimitive::icosahedron() {
    MeshData data;
    std::vector<uint> indices{
        2, 1, 0, 3, 2, 0, 4, 3, 0, 5, 4, 0, 1, 5, 0, 11, 6, 7, 11, 7,
        8, 11, 8, 9, 11, 9, 10, 11, 10, 6, 1, 2, 6, 2, 3, 7, 3, 4, 8, 4,
        5, 9, 5, 1, 10, 2, 7, 6, 3, 8, 7, 4, 9, 8, 5, 10, 9, 1, 6, 10};

    std::vector<glm::vec3> positions{
        {0.000f, 0.000f, 1.000f},
        {0.894f, 0.000f, 0.447f},
        {0.276f, 0.851f, 0.447f},
        {-0.724f, 0.526f, 0.447f},
        {-0.724f, -0.526f, 0.447f},
        {0.276f, -0.851f, 0.447f},
        {0.724f, 0.526f, -0.447f},
        {-0.276f, 0.851f, -0.447f},
        {-0.894f, 0.000f, -0.447f},
        {-0.276f, -0.851f, -0.447f},
        {0.724f, -0.526f, -0.447f},
        {0.000f, 0.000f, -1.000f}};

    auto normals = positions;
    std::vector<glm::vec2> uvs(positions.size());
    data.m_vertices = positions;
    data.m_normals = normals;
    data.m_uvs = std::move(uvs);

    data.m_indices = std::move(indices);

    return data;
}

MeshData MeshDataPrimitive::plane(float scale) {
    std::vector<glm::vec3> positions{{-1, -1, 0}, {1, -1, 0}, {1, 1, 0}, {-1, -1, 0}, {1, 1, 0}, {-1, 1, 0}};
    std::vector<glm::vec3> normals{{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}};
    std::vector<glm::vec2> texcoords{{0, 0}, {1, 1}, {1, 0}, {0, 0}, {0, 1}, {1, 1}};
    for (auto &p : positions) {
        p *= scale;
    }
    MeshData data;
    data.m_vertices = std::move(positions);
    data.m_normals = std::move(normals);
    data.m_uvs = std::move(texcoords);
    data.m_indices.resize(6);

    for (uint i = 0; i < 6; i++)
        data.m_indices[i] = i;

    return data;
}

MeshData MeshDataPrimitive::sphere(uint resolution) {
#define F(x) static_cast<float>(x)

    const float radius = 1.f;
    const uint numMeridian = resolution * 2; // resolution of mesh
    const uint numParallel = resolution;     // resolution of mesh
    float parralelDivider = 2.0f * F(M_PI) / (F(numMeridian));
    float meridianDivider = F(M_PI) / (F(numParallel)) * .9999f;

    std::vector<glm::vec2> parallel(numMeridian + 1);
    for (uint i = 0; i <= numMeridian; i++)
        parallel[i] = glm::vec2(cos(F(i) * parralelDivider), sin(F(i) * parralelDivider)) * radius; // zero circle

    std::vector<glm::vec3> positions, normals;
    positions.reserve((numParallel + 1) * (numMeridian + 1));
    for (uint j = 0; j <= numParallel; j++)
        for (uint i = 0; i <= numMeridian; i++) {
            auto p = parallel[i] * sin(F(j) * meridianDivider);
            positions.emplace_back(p, radius * cos(F(j) * meridianDivider));
        }

    normals = positions;

    std::vector<uint> indices;
    for (uint i = 0; i < numParallel; i++)
        for (uint j = 0; j < numMeridian; j++) {
            auto offset__ = (i + 0) * (numMeridian + 1) + j + 0;
            auto offset_j = (i + 0) * (numMeridian + 1) + j + 1;
            auto offset_i = (i + 1) * (numMeridian + 1) + j + 0;
            auto offsetij = (i + 1) * (numMeridian + 1) + j + 1;
            indices.insert(indices.end(), {offset__, offset_i, offset_j});
            indices.insert(indices.end(), {offset_j, offset_i, offsetij});
        }

    std::vector<glm::vec2> texCoords(positions.size());
    for (uint i = 0; i <= numMeridian; i++)
        for (uint j = 0; j <= numParallel; j++) {
            texCoords[j * (numMeridian + 1) + i] = {F(i) / F(numMeridian), F(j) / F(numParallel)};
        }

    MeshData data;
    data.m_vertices = std::move(positions);
    data.m_normals = std::move(normals);
    data.m_uvs = std::move(texCoords);
    data.m_indices = std::move(indices);
    return data;
}

MeshData MeshDataPrimitive::cube(glm::vec3 scale) {

    std::vector<glm::vec3> positions{
        {-1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f, 1.0f, -1.0f},

        {1.0f, 1.0f, 1.0f},
        {-1.0f, 1.0f, 1.0f},
        {-1.0f, -1.0f, 1.0f},
        {-1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},

        {-1.0f, 1.0f, 1.0f},
        {-1.0f, 1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, 1.0f},
        {-1.0f, 1.0f, 1.0f},

        {1.0f, 1.0f, 1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f},

        {-1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f},
        {-1.0f, -1.0f, 1.0f},
        {-1.0f, -1.0f, -1.0f},

        {-1.0f, 1.0f, -1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, -1.0f},
        {1.0f, 1.0f, 1.0f},
        {-1.0f, 1.0f, -1.0f},
        {-1.0f, 1.0f, 1.0f},
    };

    std::vector<glm::vec3> normals{
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},

        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},

        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},

        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},

        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},

        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}};

    std::vector<glm::vec2> texcoords = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}};

    for (auto &p : positions)
        p *= scale;

    MeshData data;

    data.m_vertices = std::move(positions);
    data.m_normals = std::move(normals);
    data.m_uvs = std::move(texcoords);
    data.m_indices.resize(36);

    for (uint i = 0; i < 36; i++)
        data.m_indices[i] = i;

    return data;
}
} // namespace Visual
