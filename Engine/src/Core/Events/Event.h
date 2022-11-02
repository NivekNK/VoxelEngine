#pragma once

#include <eventpp/eventdispatcher.h>
#include <fmt/format.h>

#include "Utilities/Utils.h"

namespace nk {
    enum class EventType {
        None = 0,
        WindowClose,
        WindowResize,
        WindowFocus,
        WindowLostFocus,
        WindowMoved,
        AppTick,
        AppUpdate,
        AppRender,
        KeyPressed,
        KeyReleased,
        KeyTyped,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseScrolled
    };

    enum EventCategory {
        None = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput = BIT(1),
        EventCategoryKeyboard = BIT(2),
        EventCategoryMouse = BIT(3),
        EventCategoryMouseButton = BIT(4)
    };

#define EVENT_CLASS_TYPE(type)                        \
    static EventType GetStaticType() { return EventType::type; }                             \
    virtual EventType GetEventType() const override { \
        return GetStaticType();                       \
    }                                                 \
    virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
    virtual int GetCategoryFlags() const override { return category; }

    class Event {
    public:
        bool Handled = false;

        [[nodiscard]] virtual EventType GetEventType() const = 0;
        [[nodiscard]] virtual const char* GetName() const = 0;
        [[nodiscard]] virtual i32 GetCategoryFlags() const = 0;
        [[nodiscard]] virtual std::string ToString() const { return GetName(); }

        [[nodiscard]] bool IsInCategory(const EventCategory category) const {
            return GetCategoryFlags() & category;
        }
    };

    class EventDispatcher {
    public:
        struct EventPolicies {
            static EventType getEvent(
                const Event& e, const EventType eventType
            ) {
                if (e.GetEventType() == eventType)
                    return e.GetEventType();

                return EventType::None;
            }
        };

        using Dispatcher = eventpp::EventDispatcher<
            EventType, void(Event&, EventType), EventPolicies>;

        template<
            typename T,
            typename = std::enable_if_t<std::is_base_of_v<Event, T>, T>>
        using Callback = std::function<bool(T&)>;

        EventDispatcher() = default;

        template<
            typename T,
            typename = std::enable_if_t<std::is_base_of_v<Event, T>, T>>
        Dispatcher::Handle AddListener(const Callback<T>& callback) {
            return m_Dispatcher.appendListener(
                T::GetStaticType(),
                [callback](Event& event, const EventType _) {
                    auto e = dynamic_cast<T&>(event);
                    event.Handled = callback(e);
                }
            );
        }

        bool RemoveListener(
            const EventType eventType, const Dispatcher::Handle& handle
        ) {
            return m_Dispatcher.removeListener(eventType, handle);
        }

        template<
            typename T,
            typename = std::enable_if_t<std::is_base_of_v<Event, T>, T>>
        bool RemoveListener(const Dispatcher::Handle& handle) {
            return m_Dispatcher.removeListener(T::GetStaticType(), handle);
        }

        Dispatcher GetDispatcher() const { return m_Dispatcher; }

        void Dispatch(Event& event, const EventType eventType) const {
            m_Dispatcher.dispatch(event, eventType);
        }

        template<
            typename T,
            typename = std::enable_if_t<std::is_base_of_v<Event, T>, T>>
        void Dispatch(T& event, const EventType eventType) const {
            m_Dispatcher.dispatch(event, eventType);
        }

        template<
            typename T,
            typename = std::enable_if_t<std::is_base_of_v<Event, T>, T>>
        void Dispatch(T& event) const {
            m_Dispatcher.dispatch(event, event.GetEventType());
        }

        template<
            typename T,
            typename = std::enable_if_t<std::is_base_of_v<Event, T>, T>>
        void Dispatch(Event& event) const {
            m_Dispatcher.dispatch(event, T::GetStaticType());
        }

    private:
        Dispatcher m_Dispatcher;
    };
}  // namespace nk

template<typename T>
struct fmt::formatter<
    T, std::enable_if_t<std::is_base_of_v<nk::Event, T>, char>>
    : formatter<std::string> {
    template<typename FormatContext>
    auto format(const nk::Event& e, FormatContext& ctx) {
        return formatter<std::string>::format(e.ToString(), ctx);
    }
};
