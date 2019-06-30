#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include "glm/glm.hpp"

#include <iostream>

#include "entityx/entityx.h"

namespace ex = entityx;

class Space {
  public:

    struct Transform {
      public:
        Transform() : voxel(0), matrix(1) {}
        Transform(const glm::dvec3 &pos) : matrix(1) {
            auto vox = glm::floor((pos + halfBoxSize) / boxSize);
            voxel = static_cast<glm::ivec3>(vox);
            glm::vec3 offset = pos - vox * boxSize;
            matrix = glm::translate(matrix, offset);
        }

        Transform(const glm::dmat4 &dmat) {
            glm::dvec3 pos = dmat[3];
            auto vox = glm::floor((pos + halfBoxSize) / boxSize);
            voxel = static_cast<glm::ivec3>(vox);
            glm::vec3 offset = pos - vox * boxSize;
            matrix = static_cast<glm::mat4>(dmat);
            matrix[3] = glm::vec4(offset, dmat[3][3]);
        };

        void print() {
            std::cout << "Voxel: " << glm::to_string(voxel)
                      << "  Local: " << glm::to_string(matrix) << std::endl;
        }

        static Transform toLocal(const glm::dmat4 &mat) {
            return Transform(mat);
        };

        static glm::dmat4 toGlobal(const Transform &tr) {
            glm::dmat4 res(tr.matrix);
            res = glm::translate(res, glm::dvec3(tr.voxel) * Space::boxSize);
            return res;
        };

        glm::ivec3 voxel;
        glm::mat4 matrix;
    };

    struct Container {
        Container(int) {}
        Container() = default;
        bool empty() {
            return staticEntities.empty() && dynamicEntities.empty();
        }
        std::vector<ex::Entity> staticEntities;
        std::vector<ex::Entity> dynamicEntities;
    };

    void addEntity(ex::Entity &e, Transform pos) {
        //auto &place = m_tree(pos.voxel);
        //place.staticEntities.push_back(e);
    }

    void goOutOfContainer(glm::ivec3 pos) {
        auto found = m_tree.find(pos);
        if (found != m_tree.end()) {
            auto &container = found->second;
            container.dynamicEntities.clear();
            if (container.staticEntities.empty())
                m_tree.erase(found);
        }
    }

    static constexpr double BOXSIZE = 100.;
    static constexpr glm::dvec3 boxSize = glm::dvec3(BOXSIZE);
    static constexpr glm::dvec3 halfBoxSize = glm::dvec3(BOXSIZE / 2);

  private:
    //Octree<Container> m_tree;
    std::unordered_map<glm::ivec3, Container> m_tree;
};