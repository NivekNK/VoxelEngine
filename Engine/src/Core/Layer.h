#pragma once

#include "Core/Events/Event.h"

namespace nk {
    class Layer {
    public:
        Layer(const std::string& debug_name = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(f32 delta_time) {}
        virtual void OnEvent(Event& event) {}

        const std::string& GetName() const { return m_DebugName; }

    private:
        std::string m_DebugName;

        EventDispatcher m_Dispatcher;
    };
}  // namespace nk
