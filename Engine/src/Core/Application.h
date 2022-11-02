#pragma once

#include "Core/Events/ApplicationEvent.h"
#include "Core/LayerStack.h"
#include "Core/Window.h"

namespace nk {
    class Renderer;

    class Application {
    public:
        Application();
        virtual ~Application();

        void Run();
        void OnEvent(Event& event);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        Window& GetWindow() const { return *m_Window; }

        static Application& Get() { return *s_Instance; }

    private:
        bool OnWindowClose(WindowCloseEvent& event);
        bool OnWindowResize(WindowResizeEvent& event);

        bool m_Running = true;

        std::shared_ptr<Window> m_Window;
        LayerStack m_LayerStack;
        EventDispatcher m_Dispatcher;
        std::unique_ptr<Renderer> m_Renderer;

        static Application* s_Instance;
    };

    Application* CreateApplication();
} // namespace nk
