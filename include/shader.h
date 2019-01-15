#ifndef SHADER_H
#define SHADER_H

namespace Visual {

class Shader {
    typedef unsigned int GLuint;
    typedef int GLint;
    GLuint m_program;

  public:
    Shader(const char *vertex_file_path, const char *fragment_file_path);
    void use() const;
    void setMat4(int index, const float *const mat) const;
    void setMat4(const char *var, const float *const mat) const;
    GLuint getProgram() const { return m_program; }
    GLint getLocation(const char *var) const;

    ~Shader();
};
} // namespace Visual
#endif // SHADER_H
