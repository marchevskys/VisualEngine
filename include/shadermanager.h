#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <vector>

namespace Visual {
class Shader;

class ShaderManager {
    std::vector<Shader> shaders;
    ShaderManager();
    ~ShaderManager();

  public:
    static ShaderManager *get() {
        static ShaderManager inst;
        return &inst;
    }
    const Shader *flatShader() const;
    const std::vector<Shader> *getAll3DShaders() { return &shaders; }
    void setViewMatricesForAllShaders(const double *mat);
    void setProjectionMatricesForAllShaders(const float *mat);
    void setShadowMatricesForAllShaders(const float *mat);
    void setViewPos(const double *dir);
};
} // namespace Visual
#endif // SHADERMANAGER_H
