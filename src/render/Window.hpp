#pragma once

#include <string>

struct GLFWwindow;

namespace render {

// Owns the GLFW window and its OpenGL 3.3 core context. Throws std::runtime_error on failure.
class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void close();
    void swapBuffers();
    void setTitle(const std::string& title);
    void framebufferSize(int& width, int& height) const;

    GLFWwindow* handle() const { return window_; }

private:
    GLFWwindow* window_ = nullptr;
};

} // namespace render
