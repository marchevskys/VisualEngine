#include "meshdata.h"
#include "logger.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <math.h>
namespace Visual {
std::vector<float> MeshData::makeSingleArray() const {
    std::vector<float> array;

    switch (m_type) {
    case MeshData::Type::V:
        array = m_vertices;
        break;

    case MeshData::Type::VN:
        if (m_vertices.size() != m_normals.size()) {
            DLOGN(m_vertices.size(), m_normals.size());
            THROW("BAD VN");
        }
        array.reserve(m_vertices.size() + m_normals.size());
        for (size_t i = 0; i < m_vertices.size(); i += 3) {
            array.push_back(m_vertices[i]);
            array.push_back(m_vertices[i + 1]);
            array.push_back(m_vertices[i + 2]);
            array.push_back(m_normals[i]);
            array.push_back(m_normals[i + 1]);
            array.push_back(m_normals[i + 2]);
        }
        break;

    case MeshData::Type::VT:
        array.reserve(m_vertices.size() + m_vertices.size());
        for (size_t i = 0; i < m_vertices.size(); i += 3) {
            array.push_back(m_vertices[i]);
            array.push_back(m_vertices[i + 1]);
            array.push_back(m_vertices[i + 2]);
            array.push_back(m_uvs[i]);
            array.push_back(m_uvs[i + 1]);
        }
        break;

    case MeshData::Type::VTN:
        if (m_vertices.size() != m_normals.size() || m_vertices.size() != m_uvs.size() / 2 * 3) {
            DLOGN(m_vertices.size(), m_normals.size(), m_uvs.size());
            THROW("BAD VTN");
        }
        array.reserve(m_vertices.size() + m_normals.size() + m_uvs.size());
        for (size_t i = 0, j = 0; i < m_vertices.size(); i += 3) {
            array.push_back(m_vertices[i]);
            array.push_back(m_vertices[i + 1]);
            array.push_back(m_vertices[i + 2]);
            array.push_back(m_normals[i]);
            array.push_back(m_normals[i + 1]);
            array.push_back(m_normals[i + 2]);
            array.push_back(m_uvs[j++]);
            array.push_back(m_uvs[j++]);
        }

        break;
    //case MeshData::Type::VTNTB:
    //break;
    default:
        THROW("Bad mesh type");
    }
    return array;
}

MeshData::MeshData(MeshData &&other) {
    m_indices = std::move(other.m_indices);
    m_vertices = std::move(other.m_vertices);
    m_normals = std::move(other.m_normals);
    m_uvs = std::move(other.m_uvs);
    m_type = other.m_type;
}

MeshData::MeshData(const MeshData &other) {
    THROW("copied");
}

void MeshData::scale(float k) {
    for (auto &v : m_vertices)
        v *= k;
}

template <>
const std::vector<float> MeshData::getVertices() { return m_vertices; }

template <>
const std::vector<double> MeshData::getVertices() {
    std::vector<double> vec(m_vertices.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        vec[i] = static_cast<double>(m_vertices[i]);
    }
    return vec;
}

MeshData MeshPrimitives::icosahedron(MeshData::Type type) {
    MeshData data;
    std::vector<uint> indices{
        2, 1, 0, 3, 2, 0, 4, 3, 0, 5, 4, 0, 1, 5, 0, 11, 6, 7, 11, 7,
        8, 11, 8, 9, 11, 9, 10, 11, 10, 6, 1, 2, 6, 2, 3, 7, 3, 4, 8, 4,
        5, 9, 5, 1, 10, 2, 7, 6, 3, 8, 7, 4, 9, 8, 5, 10, 9, 1, 6, 10};

    std::vector<float> positions{
        0.000f, 0.000f, 1.000f,
        0.894f, 0.000f, 0.447f,
        0.276f, 0.851f, 0.447f,
        -0.724f, 0.526f, 0.447f,
        -0.724f, -0.526f, 0.447f,
        0.276f, -0.851f, 0.447f,
        0.724f, 0.526f, -0.447f,
        -0.276f, 0.851f, -0.447f,
        -0.894f, 0.000f, -0.447f,
        -0.276f, -0.851f, -0.447f,
        0.724f, -0.526f, -0.447f,
        0.000f, 0.000f, -1.000f};

    auto normals = positions;
    std::vector<float> uvs(positions.size() / 3 * 2, 0.2);
    data.m_vertices = positions;
    data.m_normals = normals;
    data.m_uvs = std::move(uvs);

    data.m_indices = std::move(indices);
    data.m_type = type;
    return data;
}

MeshData MeshPrimitives::plane(float scale, MeshData::Type type) {
    std::vector<float> positions{-1, -1, 0, 1, 1, 0, 1, -1, 0, -1, -1, 0, -1, 1, 0, 1, 1, 0};
    std::vector<float> normals{0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1};
    std::vector<float> texcoords{0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1};
    for (auto &p : positions) {
        p *= scale;
    }
    MeshData data;
    data.m_vertices = std::move(positions);
    data.m_normals = std::move(normals);
    data.m_uvs = std::move(texcoords);
    data.m_indices.resize(6);
    data.m_type = type;
    for (uint i = 0; i < 6; i++)
        data.m_indices[i] = i;

    return data;
}

MeshData MeshPrimitives::sphere(uint resolution, MeshData::Type type) {
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
            positions.emplace_back(p[0], p[1], radius * cos(F(j) * meridianDivider));
            //positions.insert(positions.end(), {p[0], p[1], radius * cos(F(j) * meridianDivider)});
        }

    normals = positions;

    std::vector<uint> indices;
    for (uint i = 0; i < numParallel; i++)
        for (uint j = 0; j < numMeridian; j++) {
            auto offset__ = (i + 0) * (numMeridian + 1) + j + 0;
            auto offset_j = (i + 0) * (numMeridian + 1) + j + 1;
            auto offset_i = (i + 1) * (numMeridian + 1) + j + 0;
            auto offsetij = (i + 1) * (numMeridian + 1) + j + 1;
            indices.insert(indices.end(), {offset__, offset_j, offset_i});
            indices.insert(indices.end(), {offset_j, offsetij, offset_i});
        }

    std::vector<glm::vec2> uvs(positions.size());
    for (uint i = 0; i <= numMeridian; i++)
        for (uint j = 0; j <= numParallel; j++) {
            uvs[j * (numMeridian + 1) + i] = {F(i) / F(numMeridian), F(j) / F(numParallel)};
        }

    MeshData data;
    for (const auto &p : positions)
        data.m_vertices.insert(data.m_vertices.end(), {p[0], p[1], p[2]});
    for (const auto &n : normals)
        data.m_normals.insert(data.m_normals.end(), {n[0], n[1], n[2]});
    for (const auto &t : uvs)
        data.m_uvs.insert(data.m_uvs.end(), {t[0], t[1]});

    data.m_indices = std::move(indices);
    data.m_type = type;

    return data;
}

MeshData MeshPrimitives::cube(float scale_x, float scale_y, float scale_z, MeshData::Type type) {
    std::vector<float> positions{
        -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f};

    std::vector<float> normals{
        0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

    std::vector<float> texcoords = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f

    };

    for (size_t i = 0; i < positions.size(); i += 3) {
        positions[i] *= scale_x;
        positions[i + 1] *= scale_y;
        positions[i + 2] *= scale_z;
    }
    MeshData data;

    data.m_vertices = std::move(positions);
    data.m_normals = std::move(normals);
    data.m_uvs = std::move(texcoords);
    data.m_indices.resize(36);
    data.m_type = type;
    for (uint i = 0; i < 36; i++)
        data.m_indices[i] = i;

    return data;
}
} // namespace Visual
