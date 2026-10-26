#pragma once

#include <string>

#include <glm/glm.hpp>

namespace render {

// Compiles and links a vertex/fragment pair. Throws std::runtime_error naming the file and
// including the GL info log if anything fails.
class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const;
    void set(const char* name, const glm::mat4& value) const;
    void set(const char* name, const glm::vec3& value) const;

private:
    int location(const char* name) const;

    unsigned int program_ = 0;
};

} // namespace render
