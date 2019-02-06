#include "shader.h"
#include "logger.h"

#include <GL/glew.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
namespace Visual {
std::set<Shader *> Shader::registeredShaders;

std::string codeFromPath(const char *path) {
    std::string code;
    std::ifstream shaderStream(std::string(_SHADER_PATH) + path, std::ios::in);
    if (shaderStream.is_open()) {
        std::stringstream sstr;
        sstr << shaderStream.rdbuf();
        code = sstr.str();
        shaderStream.close();
        return code;
    }
    printf("Impossible to open %s. Are you in the right directory?\n", path);
    THROW("No shader file");
}

GLuint shaderFromCode(const std::string &code, GLint type) {
    GLuint shader = glCreateShader(type);

    char const *VertexSourcePointer = code.c_str();
    glShaderSource(shader, 1, &VertexSourcePointer, nullptr);
    glCompileShader(shader);

    int InfoLogLength;
    GLint Result = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> errorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(shader, InfoLogLength, nullptr, &errorMessage[0]);
        std::cerr << &errorMessage[0] << std::endl;
        THROW();
    }
    return shader;
}

Shader::Shader(const char *vPath, const char *fPath, const char *gPath) {
    auto vertexShader = shaderFromCode(codeFromPath(vPath), GL_VERTEX_SHADER);
    auto fragmentShader = shaderFromCode(codeFromPath(fPath), GL_FRAGMENT_SHADER);
    auto geometryShader = 0;
    if (gPath)
        geometryShader = shaderFromCode(codeFromPath(gPath), GL_GEOMETRY_SHADER);

    // Link the program
    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    if (gPath)
        glAttachShader(m_program, geometryShader);

    glLinkProgram(m_program);

    int InfoLogLength;
    GLint Result = GL_FALSE;
    glGetProgramiv(m_program, GL_LINK_STATUS, &Result);
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(m_program, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        std::cerr << &ProgramErrorMessage[0] << std::endl;
        THROW();
    }

    glDetachShader(m_program, vertexShader);
    glDetachShader(m_program, fragmentShader);

    if (gPath) {
        glDetachShader(m_program, geometryShader);
        glDeleteShader(geometryShader);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    use();
}

void Shader::use() const {
    glUseProgram(m_program);
}

void Shader::setInt(int location, const int value) const {
    glUniform1i(location, value);
}

void Shader::setFloat(int location, const float value) const {
    glUniform1f(location, value);
}

void Shader::setVec3(int location, const double *const vec) const {
    float fVec[16];
    for (int i = 0; i < 3; i++)
        fVec[i] = static_cast<float>(vec[i]);
    glUniform3fv(location, 1, fVec);
}

void Shader::setVec4(int location, const float *const vec) const {
    glUniform4fv(location, 1, vec);
}

void Shader::setVec3(int location, const float *const vec) const {
    glUniform3fv(location, 1, vec);
}

void Shader::setMat4(int location, const float *const mat) const {
    glUniformMatrix4fv(location, 1, GL_FALSE, mat);
}

void Shader::setMat4(int location, const double *const mat) const {

    float fMat[16];
    for (int i = 0; i < 16; i++)
        fMat[i] = static_cast<float>(mat[i]);
    glUniformMatrix4fv(location, 1, GL_FALSE, fMat);
}

Shader::GLint Shader::getLocation(const char *var) const {
    GLint location = glGetUniformLocation(m_program, var);
    //    if (location == -1) {
    //        std::cerr << "No variable \"" << var << "\" in shader "
    //                  << m_program << " " << m_name << std::endl;
    //        THROW();
    //    }
    return location;
}

Shader::~Shader() {
    glDeleteProgram(m_program);
}

} // namespace Visual
