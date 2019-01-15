#include "shadermanager.h"
#include "logger.h"
#include "shader.h"
namespace Visual {
ShaderManager::ShaderManager() {
    shaders.reserve(10);
    shaders.emplace_back("../GameTest2/src/shaders/vertex.glsl", "../GameTest2/src/shaders/fragment.glsl");
    DLOG("Shaders initialized");
}

//void ShaderManager::setViewMatricesForAllShaders(const double *mat) {
//    auto &shaders = *ShaderManager::get()->getAll3DShaders();
//    for (const auto &s : shaders)
//        s.setMat4(1, mat);
//}

void ShaderManager::setProjectionMatricesForAllShaders(const float *mat) {
    auto &shaders = *ShaderManager::get()->getAll3DShaders();
    for (const auto &s : shaders)
        s.setMat4(2, mat);
}

//void ShaderManager::setViewPos(const double *dir) {
//    auto &shaders = *ShaderManager::get()->getAll3DShaders();
//    for (const auto &s : shaders)
//        s.setVec(3, dir);
//}

ShaderManager::~ShaderManager() { DLOG("Shaders destroyed"); }

const Shader *ShaderManager::flatShader() const { return &shaders[0]; }
} // namespace Visual
