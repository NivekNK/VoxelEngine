#pragma once

#include "Event.h"

namespace nk {
    class WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(const i32 width, const i32 height)
            : m_Width(width), m_Height(height) {}

        [[nodiscard]] i32 GetWidth() const { return m_Width; }
        [[nodiscard]] i32 GetHeight() const { return m_Height; }

        [[nodiscard]] virtual std::string ToString() const override {
            return fmt::format("{}: {}, {}", GetName(), m_Width, m_Height);
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    private:
        i32 m_Width;
        i32 m_Height;
    };

    class WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppTickEvent : public Event {
    public:
        AppTickEvent() = default;

        EVENT_CLASS_TYPE(AppTick)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppUpdateEvent : public Event {
    public:
        AppUpdateEvent() = default;

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppRenderEvent : public Event {
    public:
        AppRenderEvent() = default;

        EVENT_CLASS_TYPE(AppRender)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
}  // namespace nk
