#include "shader.h"
#include "logger.h"

#include <GL/glew.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
namespace Visual {
Shader::Shader(const char *vertex_file_path, const char *fragment_file_path) {

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    } else {
        printf("Impossible to open %s. Are you in the right directory?\n", vertex_file_path);
        THROW("No shader file");
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    char const *VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
        THROW();
    }

    // Compile Fragment Shader
    char const *FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    m_program = glCreateProgram();
    glAttachShader(m_program, VertexShaderID);
    glAttachShader(m_program, FragmentShaderID);
    glLinkProgram(m_program);

    // Check the program
    glGetProgramiv(m_program, GL_LINK_STATUS, &Result);
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(m_program, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
        THROW();
    }

    glDetachShader(m_program, VertexShaderID);
    glDetachShader(m_program, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
}

void Shader::use() const {
    glUseProgram(m_program);
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

void Shader::setVec3(int location, const double *const vec) const {
    float fVec[16];
    for (int i = 0; i < 3; i++)
        fVec[i] = static_cast<float>(vec[i]);
    glUniform3fv(location, 1, fVec);
}

void Shader::setVec3(int location, const float *const vec) const {
    glUniform3fv(location, 1, vec);
}

void Shader::setInt(int location, const int value) const {
    glUniform1i(location, value);
}

void Shader::setInt(const char *name, const int value) const {
    glUniform1i(glGetUniformLocation(m_program, name), value);
}

//void Shader::setMat4(const char *var, const float *const mat) const {
//    auto loc = glGetUniformLocation(m_program, var);
//    glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
//}

Shader::GLint Shader::getLocation(const char *var) const { return glGetUniformLocation(m_program, var); }

Shader::~Shader() {
    glDeleteProgram(m_program);
}
} // namespace Visual
