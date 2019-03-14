#ifndef SHADER_H
#define SHADER_H
#include <glm/glm.hpp>

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

    Shader(const char *vPath, const char *fPath, const char *gPath = nullptr);
    Shader(Shader &&other) = delete;
    Shader(const Shader &other) = delete;
    ~Shader();

    static void set(const GLint &location, const int value);
    static void set(const GLint &location, const float value);
    static void set(const GLint &location, const glm::vec3 &value);
    static void set(const GLint &location, const glm::dvec3 &value);
    static void set(const GLint &location, const glm::vec4 &value);
    static void set(const GLint &location, const glm::dvec4 &value);
    static void set(const GLint &location, const glm::mat4 &value);
    static void set(const GLint &location, const glm::dmat4 &value);

    static void set(const GLint &location, const float *valuePtr, GLint count);
    static void set(const GLint &location, const glm::vec3 *valuePtr, GLint count);
    static void set(const GLint &location, const glm::mat4 *valuePtr, GLint count);

    GLuint getProgram() const { return m_program; }
    GLint getLocation(const char *var) const;
    static std::set<Shader *> registeredShaders;

  public:
    void use() const;
};

class ShaderScreenQuad : public Shader {
  protected:
    GLint locationAndScaleId, textureLocationId;
    ShaderScreenQuad(const char *v, const char *f) : Shader(v, f) {
        // locationAndScaleId = getLocation("locationAndScale");
        textureLocationId = getLocation("tex1");
    };
    ~ShaderScreenQuad(){};

  public:
    static ShaderScreenQuad *get() {
        static ShaderScreenQuad shader("passthrough.vert", "passthrough.frag");
        return &shader;
    }
    //inline void setLocationAndScale(const float *mat) const { setMat4(locationAndScaleId, mat); };
    inline void setTexture(const int tex) const { set(textureLocationId, tex); };
};

class ShaderShadow : public Shader {
  protected:
    GLint viewProjectionId, modelId;
    ShaderShadow(const char *v, const char *f) : Shader(v, f) {
        viewProjectionId = getLocation("viewProjection");
        modelId = getLocation("model");
    };
    ~ShaderShadow(){};

  public:
    const static ShaderShadow *get() {
        static ShaderShadow shader("shadow.vert", "shadow.frag");
        return &shader;
    }
    inline void setViewProjection(const glm::mat4 &mat) const { set(viewProjectionId, mat); };

    template <class Type>
    inline void setModel(const Type &mat) const { set(modelId, mat); };
};

class Shader3d : public Shader {
  protected:
    GLint viewId, projectionId, modelId, viewPosId, lightDirId, shadowMapId,
        cascadeTransformsID, cascadeSplitsID, shadowCascadeCoutID;
    Shader3d(const char *v, const char *f) : Shader(v, f) {
        m_name = "shader3d";
        modelId = getLocation("model");
        viewId = getLocation("view");
        projectionId = getLocation("projection");
        shadowMapId = getLocation("shadowMap");

        viewPosId = getLocation("viewPos");
        lightDirId = getLocation("lightDir");
        cascadeTransformsID = getLocation("cascadeTransforms");
        cascadeSplitsID = getLocation("cascadeSplits");
        shadowCascadeCoutID = getLocation("shadowCascadeCout");
    }
    virtual ~Shader3d(){};

  public:
    void setProjection(const glm::mat4 &mat) const { set(projectionId, mat); }
    void setModel(const glm::mat4 &mat) const { set(modelId, mat); }

    void setView(const glm::mat4 &mat) const { set(viewId, mat); }
    void setViewPos(const glm::vec3 &pos) const { set(viewPosId, pos); }

    void setLightDir(const glm::vec3 &vec) const { set(lightDirId, vec); }
    void setShadowMap(const int texture) const { set(shadowMapId, texture); }

    void setCascades(const glm::mat4 *mat, const float *splits, GLint count) const {
        set(shadowCascadeCoutID, count);
        set(cascadeTransformsID, mat, count);
        set(cascadeSplitsID, splits, count);
    }
};

class ShaderPlain : public Shader3d {
  protected:
    GLint diffuseColorId;
    ShaderPlain(const char *v, const char *f) : Shader3d(v, f) {
        m_name = "Plain";
        diffuseColorId = getLocation("diffuseColor");
    }

  public:
    const static Shader3d *get() {
        static ShaderPlain shader("PBR.vert", "solid.frag");
        return &shader;
    }
    void setDiffuseColor(const glm::vec3 &color) const {
        set(diffuseColorId, color);
    };
};

class ShaderPBR : public Shader3d {
  protected:
    GLint diffuseColorId;
    ShaderPBR(const char *v, const char *f) : Shader3d(v, f) {
        m_name = "PBR";
        diffuseColorId = getLocation("diffuseColor");
    }

  public:
    const static Shader3d *get() {
        static ShaderPBR shader("PBR.vert", "PBR.frag");
        return &shader;
    }
    void setDiffuseColor(const glm::vec3 &color) const {
        set(diffuseColorId, color);
    };
};

} // namespace Visual
#endif // SHADER_H
