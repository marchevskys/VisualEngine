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
        std::cerr << &ProgramErrorMessage[0] << " in shader " << m_name << std::endl;
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

void Shader::set(const GLint &location, const int value) {
    glUniform1i(location, value);
}

void Shader::set(const GLint &location, const float value) {
    glUniform1f(location, value);
}

void Shader::set(const GLint &location, const glm::vec3 &value) {
    glUniform3fv(location, 1, &value[0]);
}

void Shader::set(const GLint &location, const glm::dvec3 &value) {
    glm::vec3 fValue(value);
    set(location, fValue);
}

void Shader::set(const GLint &location, const glm::vec4 &value) {
    glUniform4fv(location, 1, &value[0]);
}

void Shader::set(const GLint &location, const glm::dvec4 &value) {
    glm::vec4 fValue(value);
    set(location, fValue);
}

void Shader::set(const GLint &location, const glm::mat4 &value) {
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::set(const GLint &location, const glm::dmat4 &value) {
    glm::mat4 fValue(value);
    set(location, fValue);
}

void Shader::set(const Shader::GLint &location, const float *valuePtr, GLint count) {
    glUniform1fv(location, count, valuePtr);
}

void Shader::set(const Shader::GLint &location, const glm::vec3 *valuePtr, GLint count) {
    glUniform1fv(location, count, &valuePtr[0][0]);
}

void Shader::set(const Shader::GLint &location, const glm::mat4 *valuePtr, GLint count) {
    glUniformMatrix4fv(location, count, GL_FALSE, &valuePtr[0][0][0]);
}

} // namespace Visual
