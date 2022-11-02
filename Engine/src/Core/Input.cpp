#include "nkpch.h"

#include "Input.h"

#include "Core/Application.h"

#include <GLFW/glfw3.h>

namespace nk {
    Input* Input::s_Instance = new Input();

    bool Input::IsKeyPressedImpl(const i32 keycode) {
        const auto window = Application::Get().GetWindow().GetNativeWindow();
        const i32 state = glfwGetKey(window, keycode);

        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressedImpl(const i32 button) {
        const auto window = Application::Get().GetWindow().GetNativeWindow();
        const i32 state = glfwGetMouseButton(window, button);

        return state == GLFW_PRESS;
    }

    std::pair<f32, f32> Input::GetMousePositionImpl() {
        const auto window = Application::Get().GetWindow().GetNativeWindow();
        f64 x_pos, y_pos;
        glfwGetCursorPos(window, &x_pos, &y_pos);
        
        return { static_cast<f32>(x_pos), static_cast<f32>(y_pos) };
    }

    f32 Input::GetMouseXImpl() {
        auto[x, _] = GetMousePositionImpl();
        return x;
    }

    f32 Input::GetMouseYImpl() {
        auto[_, y] = GetMousePositionImpl();
        return y;
    }
}  // namespace nk
