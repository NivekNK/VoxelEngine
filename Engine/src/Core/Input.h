#pragma once

namespace nk {
    class Input {
    public:
        static bool IsKeyPressed(const i32 keycode) {
            return s_Instance->IsKeyPressedImpl(keycode);
        }

        static bool IsMouseButtonPressed(const i32 button) {
            return s_Instance->IsMouseButtonPressedImpl(button);
        }

        static std::pair<f32, f32> GetMousePosition() {
            return s_Instance->GetMousePositionImpl();
        }

        static f32 GetMouseX() { return s_Instance->GetMouseXImpl(); }

        static f32 GetMouseY() { return s_Instance->GetMouseYImpl(); }

    private:
        bool IsKeyPressedImpl(const i32 keycode);
        bool IsMouseButtonPressedImpl(const i32 button);
        std::pair<f32, f32> GetMousePositionImpl();
        f32 GetMouseXImpl();
        f32 GetMouseYImpl();

        static Input* s_Instance;
    };
}  // namespace nk
