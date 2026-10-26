#include "render/Shader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

namespace render {

namespace {

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("cannot open shader file: " + path);
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint compile(GLenum type, const std::string& path) {
    const std::string source = readFile(path);
    const char* src = source.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        glDeleteShader(shader);
        throw std::runtime_error("failed to compile " + path + ":\n" + log);
    }
    return shader;
}

} // namespace

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    GLuint vs = compile(GL_VERTEX_SHADER, vertexPath);
    GLuint fs = 0;
    try {
        fs = compile(GL_FRAGMENT_SHADER, fragmentPath);
    } catch (...) {
        glDeleteShader(vs);
        throw;
    }

    program_ = glCreateProgram();
    glAttachShader(program_, vs);
    glAttachShader(program_, fs);
    glLinkProgram(program_);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = 0;
    glGetProgramiv(program_, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(program_, sizeof(log), nullptr, log);
        glDeleteProgram(program_);
        throw std::runtime_error("failed to link " + vertexPath + " + " + fragmentPath + ":\n" +
                                 log);
    }
}

Shader::~Shader() {
    glDeleteProgram(program_);
}

void Shader::use() const {
    glUseProgram(program_);
}

void Shader::set(const char* name, const glm::mat4& value) const {
    glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const char* name, const glm::vec3& value) const {
    glUniform3fv(location(name), 1, glm::value_ptr(value));
}

int Shader::location(const char* name) const {
    return glGetUniformLocation(program_, name);
}

} // namespace render
