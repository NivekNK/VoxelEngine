#include "nkpch.h"

#include "Window.h"

#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace nk {
    static bool s_GLFWInitialized = false;

    static void GLFWErrorCallback(i32 error, const char* description) {
        CoreError("GLFW Error < {} > description: {}", error, description);
    }

    Window::Window(const std::string& name, u16 width, u16 height)
        : m_Data{name, width, height} {
        CoreInfo("Creating window {} - [{}. {}]", name, width, height);
        InitWindow();
    }

    Window::~Window() {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::OnUpdate(f32 delta_time) { glfwPollEvents(); }

    void Window::InitWindow() {
        if (!s_GLFWInitialized) {
            CoreAssertKeepMsg(glfwInit(), "Could not initialize GLFW.");
            glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitialized = true;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_Window = glfwCreateWindow(
            m_Data.Width, m_Data.Height, m_Data.Name.c_str(), nullptr, nullptr
        );
        glfwSetWindowUserPointer(m_Window, &m_Data);

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            const WindowData& data = GetWindowData(window);
            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(
            m_Window,
            [](GLFWwindow* window, const i32 keycode, i32 scan_code, i32 action,
               const int mods) {
                const WindowData& data = GetWindowData(window);
                switch (action) {
                    case GLFW_PRESS: {
                        KeyPressedEvent event(keycode, mods, 0);
                        data.EventCallback(event);
                        break;
                    }
                    case GLFW_RELEASE: {
                        KeyReleasedEvent event(keycode, mods);
                        data.EventCallback(event);
                        break;
                    }
                    case GLFW_REPEAT: {
                        KeyPressedEvent event(keycode, mods, 1);
                        data.EventCallback(event);
                        break;
                    }
                    default: {
                        CoreWarn(
                            "Key event performed with action [{}] is not "
                            "implemented!",
                            action
                        );
                        break;
                    }
                }
            }
        );

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, u32 keycode) {
            const WindowData& data = GetWindowData(window);
            KeyTypedEvent event(keycode, -1);
            data.EventCallback(event);
        });

        glfwSetMouseButtonCallback(
            m_Window,
            [](GLFWwindow* window, const i32 button, i32 action, i32 mods) {
                const WindowData& data = GetWindowData(window);
                switch (action) {
                    case GLFW_PRESS: {
                        MouseButtonPressedEvent event(button);
                        data.EventCallback(event);
                        break;
                    }
                    case GLFW_RELEASE: {
                        MouseButtonReleasedEvent event(button);
                        data.EventCallback(event);
                        break;
                    }
                    default: {
                        CoreWarn(
                            "Mouse event performed with action [{}] is not "
                            "implemented!",
                            action
                        );
                        break;
                    }
                }
            }
        );

        glfwSetScrollCallback(
            m_Window,
            [](GLFWwindow* window, const f64 x_offset, const double y_offset) {
                const WindowData& data = GetWindowData(window);
                MouseScrolledEvent event((x_offset), (y_offset));
                data.EventCallback(event);
            }
        );

        glfwSetCursorPosCallback(
            m_Window,
            [](GLFWwindow* window, const f64 x_pos, const f64 y_pos) {
                const WindowData& data = GetWindowData(window);
                MouseMovedEvent event((x_pos), (y_pos));
                data.EventCallback(event);
            }
        );

        glfwSetWindowSizeCallback(
            m_Window,
            [](GLFWwindow* window, const i32 width, const i32 height) {
                WindowData& data = GetWindowData(window);
                data.Width = width;
                data.Height = height;

                WindowResizeEvent event(width, height);
                data.EventCallback(event);
            }
        );
    }

    Window::WindowData& Window::GetWindowData(GLFWwindow* window) {
        return *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
    }
} // namespace nk
