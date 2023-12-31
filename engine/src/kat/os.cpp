#include "os.hpp"

namespace kat {
    std::shared_ptr<Window> Window::create(const Window::Config &config) {
        gbl::activeWindow = std::shared_ptr<Window>(new Window(config));
        gbl::appEvents.dispatch(AppEvent::Initialize);
        return gbl::activeWindow;
    }

    Window::Window(const Window::Config &config) {
        glfwInit();

        glfwDefaultWindowHints();

        GLFWmonitor* monitor = nullptr;
        glm::ivec2 resolution;
        std::optional<glm::ivec2> pos;
        switch (config.mode.index()) {
            case 0:
                resolution = std::get<glm::uvec2>(config.mode);
                break;
            case 1: {
                int count;
                GLFWmonitor** monitors = glfwGetMonitors(&count);
                Fullscreen fs = std::get<Fullscreen>(config.mode);
                GLFWmonitor* m = fs.monitorIndex < count ? monitors[fs.monitorIndex] : monitors[0];
                const GLFWvidmode* vm = glfwGetVideoMode(m);
                resolution = { vm->width, vm->height };
                glfwWindowHint(GLFW_RED_BITS, vm->redBits);
                glfwWindowHint(GLFW_GREEN_BITS, vm->greenBits);
                glfwWindowHint(GLFW_BLUE_BITS, vm->blueBits);
                glfwWindowHint(GLFW_REFRESH_RATE, vm->refreshRate);

                if (fs.exclusive) {
                    monitor = m;
                } else {
                    glfwWindowHint(GLFW_DECORATED, false);
                    glm::ivec2 p;
                    glfwGetMonitorPos(m, &p.x,&p.y);
                    pos = p;
                }
                break;
            }
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(resolution.x, resolution.y, config.title.c_str(), monitor, nullptr);

        if (pos.has_value()) {
            glfwSetWindowPos(m_Window, pos->x, pos->y);
            glfwSetWindowSize(m_Window, resolution.x, resolution.y);
        }

        glfwMakeContextCurrent(m_Window);
        gladLoadGL(glfwGetProcAddress);
    }

    Window::~Window() {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    bool Window::isOpen() const {
        return !glfwWindowShouldClose(m_Window);
    }

    void Window::update() {
        glfwSwapBuffers(m_Window);
        glfwPollEvents();

        // don't start updates unless the window is remaining open after pollEvents.
        if (isOpen()) kat::gbl::appEvents.dispatch(kat::AppEvent::Update);
    }

    glm::ivec2 Window::getSize() const {
        glm::ivec2 v;
        glfwGetFramebufferSize(m_Window, &v.x, &v.y);
        return v;
    }

    GLFWwindow *Window::operator*() const noexcept {
        return m_Window;
    }

    GLFWwindow *Window::getHandle() const noexcept {
        return m_Window;
    }

    bool Window::getKey(int key) {
        return glfwGetKey(m_Window, key) == GLFW_PRESS;
    }

    bool input::isKeyPressed(int key) {
        return gbl::activeWindow->getKey(key);
    }
}