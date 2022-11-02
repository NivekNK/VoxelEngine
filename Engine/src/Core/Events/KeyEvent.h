#pragma once

#include "Event.h"

namespace nk {
    class KeyEvent : public Event {
    public:
        [[nodiscard]] i32 GetKeyCode() const { return m_KeyCode; }
        [[nodiscard]] i32 GetMods() const { return m_Mods; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(const i32 keycode, const i32 mods)
            : m_KeyCode(keycode), m_Mods(mods) {}

        i32 m_KeyCode;
        i32 m_Mods;
    };

    class KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(
            const i32 keycode, const i32 mods, const i32 repeatCount
        )
            : KeyEvent(keycode, mods), m_RepeatCount(repeatCount) {}

        [[nodiscard]] i32 GetRepeatCunt() const { return m_RepeatCount; }

        [[nodiscard]] virtual std::string ToString() const override {
            return fmt::format(
                "{}: {} ({} repeats)", GetName(), m_KeyCode, m_RepeatCount
            );
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        int m_RepeatCount;
    };

    class KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(const i32 keycode, const i32 mods)
            : KeyEvent(keycode, mods) {}

        [[nodiscard]] virtual std::string ToString() const override {
            return fmt::format("{}: {}", GetName(), m_KeyCode);
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent {
    public:
        KeyTypedEvent(const i32 keycode, const i32 mods)
            : KeyEvent(keycode, mods) {}

        [[nodiscard]] virtual std::string ToString() const override {
            return fmt::format("{}: {}", GetName(), m_KeyCode);
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
}  // namespace nk
