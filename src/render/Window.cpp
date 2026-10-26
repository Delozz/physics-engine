#include "render/Window.hpp"

#include <cstdio>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <glad/gl.h>

namespace render {

namespace {

void onGlfwError(int code, const char* description) {
    std::fprintf(stderr, "GLFW error %d: %s\n", code, description);
}

} // namespace

Window::Window(int width, int height, const std::string& title) {
    glfwSetErrorCallback(onGlfwError);
    if (!glfwInit()) {
        throw std::runtime_error("glfwInit failed");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);

    window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("failed to create an OpenGL 3.3 core window");
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwDestroyWindow(window_);
        glfwTerminate();
        throw std::runtime_error("failed to load OpenGL functions");
    }
}

Window::~Window() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window_);
}

void Window::close() {
    glfwSetWindowShouldClose(window_, GLFW_TRUE);
}

void Window::swapBuffers() {
    glfwSwapBuffers(window_);
}

void Window::setTitle(const std::string& title) {
    glfwSetWindowTitle(window_, title.c_str());
}

void Window::framebufferSize(int& width, int& height) const {
    glfwGetFramebufferSize(window_, &width, &height);
}

} // namespace render
