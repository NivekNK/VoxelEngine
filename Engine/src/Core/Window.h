#pragma once

#include "Core/Events/Event.h"

struct GLFWwindow;

namespace nk {
    class Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>;
        
        Window(const std::string& name, u16 width, u16 height);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        void OnUpdate(f32 delta_time);

        [[nodiscard]] u16 GetWidth() const { return m_Data.Width; }
        [[nodiscard]] u16 GetHeight() const { return m_Data.Height; }
        std::pair<u16, u16> GetExtent() const { return {m_Data.Width, m_Data.Height}; }
        bool WasWindowResized() const { return m_Data.WindowResized; }
        void ResetWindowResizedFlag() { m_Data.WindowResized = false; }

        void SetEventCallback(const EventCallbackFn& callback) {
            m_Data.EventCallback = callback;
        }

        GLFWwindow* GetNativeWindow() const { return m_Window; }

    private:
        void InitWindow();

        struct WindowData {
            std::string Name;
            u16 Width;
            u16 Height;
            bool WindowResized;

            EventCallbackFn EventCallback;
        };

        static WindowData& GetWindowData(GLFWwindow* window);

        GLFWwindow* m_Window;
        WindowData m_Data;
    };
}  // namespace nk
