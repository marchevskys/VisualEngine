#ifndef SHADER_H
#define SHADER_H
#include <set>
#include <string>
#define getName(var) #var
namespace Visual {

class Shader {
  protected:
    typedef unsigned int GLuint;
    typedef int GLint;
    GLuint m_program;
    std::string m_name = "undefined";

    //public:
    Shader(const char *vPath, const char *fPath, const char *gPath = nullptr);
    Shader(Shader &&other) = delete;
    Shader(const Shader &other) = delete;
    ~Shader();

    void setInt(int location, const int value) const;
    void setFloat(int location, const float value) const;
    //template<class Precision>
    void setVec3(int location, const float *const vec) const;
    void setVec3(int location, const double *const vec) const;
    void setVec4(int location, const float *const vec) const;
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
    GLuint viewId, projectionId, modelId, viewPosId, lightDirId;
    Shader3d(const char *v, const char *f) : Shader(v, f) {
        m_name = "shader3d";
        modelId = getLocation("model");
        viewId = getLocation("view");
        projectionId = getLocation("projection");
        viewPosId = getLocation("viewPos");
        lightDirId = getLocation("lightDir");
    }
    virtual ~Shader3d(){};

  public:
    inline void setProjection(const float *mat) const { setMat4(projectionId, mat); };

    inline void setModel(const float *mat) const { setMat4(modelId, mat); };
    inline void setModel(const double *mat) const { setMat4(modelId, mat); };

    inline void setView(const float *mat) const { setMat4(viewId, mat); };
    inline void setView(const double *mat) const { setMat4(viewId, mat); };

    inline void setViewPos(const float *vec) const { setVec3(viewPosId, vec); };
    inline void setViewPos(const double *vec) const { setVec3(viewPosId, vec); };

    inline void setLightDir(const float *vec) const { setVec4(lightDirId, vec); };
};

class ShaderPBR : public Shader3d {
  protected:
    GLuint diffuseColorId;
    ShaderPBR(const char *v, const char *f) : Shader3d(v, f) {
        m_name = "PBR";
        //diffuseColorId = getLocation("diffuseColor");
    }

    ~ShaderPBR(){};

  public:
    static Shader3d *get() {
        static ShaderPBR shader("PBR.vert", "PBR.frag");
        return &shader;
    }
    inline void setDiffuseColor(const float *color) const {
        setVec3(diffuseColorId, color);
    };
};

} // namespace Visual
#endif // SHADER_H
