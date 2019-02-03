#ifndef SHADER_H
#define SHADER_H

namespace Visual {

class Shader {
    typedef unsigned int GLuint;
    typedef int GLint;
    GLuint m_program;

  public:
    Shader(const char *vPath, const char *fPath, const char *gPath = nullptr);
    void use() const;
    void setMat4(int index, const float *const mat) const;
    //void setMat4(const char *var, const float *const mat) const;
    void setMat4(int index, const double *const mat) const;
    void setVec3(int index, const double *const vec) const;
    void setVec3(int index, const float *const vec) const;
    void setInt(int index, const int value) const;
    void setInt(const char *name, const int value) const;

    GLuint getProgram() const { return m_program; }
    GLint getLocation(const char *var) const;

    ~Shader();
};

class Shader3d : public Shader {
  protected:
    Shader3d(const char *v, const char *f) : Shader(v, f) {}
};

class ShaderShadow : public Shader3d {
  protected:
    ShaderShadow(const char *v, const char *f) : Shader3d(v, f){};
    ~ShaderShadow(){};

  public:
    static Shader3d *get() {
        static ShaderShadow shader("shadow.vert", "shadow.frag");
        return &shader;
    }
};

class ShaderPBR : public Shader3d {
  protected:
    ShaderPBR(const char *v, const char *f) : Shader3d(v, f){};
    ~ShaderPBR(){};

  public:
    static Shader3d *get() {
        static ShaderPBR shader("PBR.vert", "PBR.frag");
        return &shader;
    }
};

} // namespace Visual
#endif // SHADER_H
