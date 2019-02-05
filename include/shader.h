#ifndef SHADER_H
#define SHADER_H
#include <set>

namespace Visual {

class Shader {
  protected:
    typedef unsigned int GLuint;
    typedef int GLint;
    GLuint m_program;

    //public:
    Shader(const char *vPath, const char *fPath, const char *gPath = nullptr);
    Shader(Shader &&other) = delete;
    Shader(const Shader &other) = delete;
    ~Shader();

    void setInt(int location, const int value) const;
    void setFloat(int location, const float value) const;
    void setVec3(int location, const double *const vec) const;
    void setVec3(int location, const float *const vec) const;
    void setMat4(int location, const float *const mat) const;
    void setMat4(int location, const double *const mat) const;
    GLuint getProgram() const { return m_program; }
    GLint getLocation(const char *var) const;
    static std::set<Shader *> registeredShaders;

  public:
    void use() const;
};

class ShaderShadow : public Shader {
  protected:
    GLuint ViewProjectionId;
    ShaderShadow(const char *v, const char *f) : Shader(v, f){};
    ~ShaderShadow(){};

  public:
    static Shader *get() {
        static ShaderShadow shader("shadow.vert", "shadow.frag");
        return &shader;
    }
    inline void setViewProjection(const float *mat) const { setMat4(ViewProjectionId, mat); };
};

class Shader3d : public Shader {
  protected:
    GLuint ViewId, projectionId, modelId;
    Shader3d(const char *v, const char *f) : Shader(v, f) {}
    virtual ~Shader3d(){};

  public:
    inline void setModel(const float *mat) const { setMat4(modelId, mat); };
    inline void setModel(const double *mat) const { setMat4(modelId, mat); };
};

class ShaderPBR : public Shader3d {
  protected:
    GLuint colorId, roughnessId;
    ShaderPBR(const char *v, const char *f) : Shader3d(v, f) {

        colorId = getLocation("material.color");
        roughnessId = getLocation("material.roughness");

        //        TextureID = glGetUniformLocation(programID, "myTextureSampler");

        //        DepthBiasID = glGetUniformLocation(programID, "DepthBiasMVP");
        //        ShadowMapID = glGetUniformLocation(programID, "shadowMap");
        //        lightDirID = glGetUniformLocation(programID, "lightDir");
        //        viewPosID = glGetUniformLocation(programID, "viewPos");
        //        lightColorID = glGetUniformLocation(programID, "lightColor");
    }

    ~ShaderPBR(){};

  public:
    static Shader3d *get() {
        static ShaderPBR shader("PBR.vert", "PBR.frag");
        return &shader;
    }
    inline void setColor(const float *color) const { setVec3(colorId, color); };
    inline void setRoughness(const float roughness) const { setFloat(colorId, roughness); };
};

} // namespace Visual
#endif // SHADER_H
