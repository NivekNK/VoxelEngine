#pragma once

#include "Event.h"

namespace nk {
    class MouseMovedEvent : public Event {
    public:
        MouseMovedEvent(const f32 x, const f32 y)
            : m_MouseX(x), m_MouseY(y) {}

        [[nodiscard]] f32 GetX() const { return m_MouseX; }
        [[nodiscard]] f32 GetY() const { return m_MouseY; }

        [[nodiscard]] virtual std::string ToString() const override {
            return fmt::format("{}: {}, {}", GetName(), m_MouseX, m_MouseY);
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    private:
        f32 m_MouseX;
        f32 m_MouseY;
    };

    class MouseScrolledEvent : public Event {
    public:
        MouseScrolledEvent(const f32 x_offset, const f32 y_offset)
            : m_XOffset(x_offset), m_YOffset(y_offset) {}

        [[nodiscard]] f32 GetXOffset() const { return m_XOffset; }
        [[nodiscard]] f32 GetYOffset() const { return m_YOffset; }

        [[nodiscard]] virtual std::string ToString() const override {
            return fmt::format("{}: {}, {}", GetName(), m_XOffset, m_YOffset);
        }

        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    private:
        f32 m_XOffset;
        f32 m_YOffset;
    };

    class MouseButtonEvent : public Event {
    public:
        [[nodiscard]] i32 GetMouseButton() const { return m_Button; }

        [[nodiscard]] virtual std::string ToString() const override {
            return fmt::format("{}: {}", GetName(), m_Button);
        }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    protected:
        explicit MouseButtonEvent(const i32 button) : m_Button(button) {}

        i32 m_Button;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent {
    public:
        explicit MouseButtonPressedEvent(const i32 button)
            : MouseButtonEvent(button) {}

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent {
    public:
        explicit MouseButtonReleasedEvent(const i32 button)
            : MouseButtonEvent(button) {}

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };
}  // namespace nk
