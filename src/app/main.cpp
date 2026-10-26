#include <cstdio>
#include <exception>
#include <random>
#include <string>

#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "app/Scenes.hpp"
#include "physics/World.hpp"
#include "render/Camera.hpp"
#include "render/Renderer.hpp"
#include "render/Window.hpp"

namespace {

constexpr double kDt = 1.0 / 120.0;
constexpr int kMaxStepsPerFrame = 8;
constexpr size_t kMaxBodies = 500;

struct AppState {
    phys::World world;
    std::mt19937 rng{app::kSeed};
    render::Camera camera;
    int scene = 0;
    bool paused = false;
    bool stepRequested = false;
    bool showContacts = false;
    bool atCapacity = false;
    bool dragging = false;
    double lastX = 0.0;
    double lastY = 0.0;
};

AppState& stateOf(GLFWwindow* window) {
    return *static_cast<AppState*>(glfwGetWindowUserPointer(window));
}

void switchScene(AppState& state, int index) {
    state.scene = index;
    state.atCapacity = false;
    app::loadScene(index, state.world, state.rng);
}

void onKey(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (action != GLFW_PRESS) {
        return;
    }
    AppState& state = stateOf(window);

    switch (key) {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
    case GLFW_KEY_SPACE:
        state.paused = !state.paused;
        break;
    case GLFW_KEY_N:
        state.stepRequested = state.paused;
        break;
    case GLFW_KEY_R:
        switchScene(state, state.scene);
        break;
    case GLFW_KEY_C:
        state.showContacts = !state.showContacts;
        break;
    case GLFW_KEY_B:
        if (state.world.spheres().size() >= kMaxBodies) {
            state.atCapacity = true;
        } else {
            app::spawnSpheres(state.world, state.rng, 10, 8.0, 12.0);
        }
        break;
    case GLFW_KEY_1:
    case GLFW_KEY_2:
    case GLFW_KEY_3:
    case GLFW_KEY_4:
        switchScene(state, key - GLFW_KEY_1);
        state.camera.reset();
        break;
    default:
        break;
    }
}

void onMouseButton(GLFWwindow* window, int button, int action, int /*mods*/) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }
    AppState& state = stateOf(window);
    state.dragging = action == GLFW_PRESS;
    glfwGetCursorPos(window, &state.lastX, &state.lastY);
}

void onCursorPos(GLFWwindow* window, double x, double y) {
    AppState& state = stateOf(window);
    if (!state.dragging) {
        return;
    }
    const float sensitivity = 0.3f;
    state.camera.orbit(-static_cast<float>(x - state.lastX) * sensitivity,
                       static_cast<float>(y - state.lastY) * sensitivity);
    state.lastX = x;
    state.lastY = y;
}

void onScroll(GLFWwindow* window, double /*xoffset*/, double yoffset) {
    stateOf(window).camera.zoom(static_cast<float>(yoffset));
}

std::string makeTitle(const AppState& state, double fps) {
    char buf[128];
    std::snprintf(buf, sizeof(buf), "%s | %.0f FPS | %zu bodies%s", app::sceneName(state.scene),
                  fps, state.world.spheres().size(), state.atCapacity ? " | max bodies" : "");
    return buf;
}

} // namespace

int main() {
    try {
        render::Window window(1280, 720, "Sphere Physics");
        render::Renderer renderer(SHADER_DIR);

        AppState state;
        switchScene(state, 0);

        GLFWwindow* handle = window.handle();
        glfwSetWindowUserPointer(handle, &state);
        glfwSetKeyCallback(handle, onKey);
        glfwSetMouseButtonCallback(handle, onMouseButton);
        glfwSetCursorPosCallback(handle, onCursorPos);
        glfwSetScrollCallback(handle, onScroll);

        double previous = glfwGetTime();
        double accumulator = 0.0;
        double titleTimer = 0.0;
        int frames = 0;

        while (!window.shouldClose()) {
            glfwPollEvents();

            const double now = glfwGetTime();
            const double frameTime = now - previous;
            previous = now;

            if (state.paused) {
                accumulator = 0.0;
                if (state.stepRequested) {
                    state.world.step(kDt);
                }
            } else {
                accumulator += frameTime;
                int steps = 0;
                while (accumulator >= kDt && steps < kMaxStepsPerFrame) {
                    state.world.step(kDt);
                    accumulator -= kDt;
                    ++steps;
                }
                if (steps == kMaxStepsPerFrame) {
                    accumulator = 0.0; // spiral-of-death guard: drop the backlog
                }
            }
            state.stepRequested = false;

            int width = 0;
            int height = 0;
            window.framebufferSize(width, height);
            if (width > 0 && height > 0) {
                renderer.draw(state.world, state.camera, width, height, state.showContacts);
            }
            window.swapBuffers();

            ++frames;
            titleTimer += frameTime;
            if (titleTimer >= 0.5) {
                window.setTitle(makeTitle(state, frames / titleTimer));
                frames = 0;
                titleTimer = 0.0;
            }
        }
    } catch (const std::exception& e) {
        std::fprintf(stderr, "error: %s\n", e.what());
        return 1;
    }
    return 0;
}
