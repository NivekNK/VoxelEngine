#include "nkpch.h"

#include "Application.h"

#include "Utilities/Utils.h"
#include "Core/Rendering/Renderer.h"

namespace nk {
    Application* Application::s_Instance = nullptr;

    Application::Application() {
        CoreInfo("Creating application.");
        CoreAssertMsg(s_Instance == nullptr, "Application already exists!");
        s_Instance = this;

        // TODO: move window initialization so initial width, hight and name can be configured
        m_Window = std::make_shared<Window>("NKEngine", 800, 600);
        CoreAssertMsg(m_Window, "Application window not correctly created!");
        m_Window->SetEventCallback([this](auto&& arg) {
            Application::OnEvent(std::forward<decltype(arg)>(arg));
        });

        m_Renderer = std::make_unique<Renderer>("NKEngine", m_Window);

        m_Dispatcher.AddListener<WindowCloseEvent>(
            NK_BIND_EVENT_FN(Application::OnWindowClose)
        );
    }

    Application::~Application() {
    }

    void Application::Run() {
        while (m_Running) {
            for (Layer* layer : m_LayerStack)
                layer->OnUpdate(0);

            m_Window->OnUpdate(0);

            m_Renderer->Draw();
        }
    }

    void Application::OnEvent(Event& event) {
        m_Dispatcher.Dispatch<WindowCloseEvent>(event);

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
            if (event.Handled)
                break;

            (*it)->OnEvent(event);
        }
    }

    void Application::PushLayer(Layer* layer) { m_LayerStack.PushLayer(layer); }

    void Application::PushOverlay(Layer* overlay) {
        m_LayerStack.PushOverlay(overlay);
    }

    bool Application::OnWindowClose(WindowCloseEvent& event) {
        m_Running = false;
        return true;
    }
}  // namespace nk
