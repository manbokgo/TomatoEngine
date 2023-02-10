#include "pch.h"
#include "Input.h"

#include "Application.h"

namespace tomato
{
    bool Input::IsKeyPressed(KeyCode key)
    {
        return true;
    }
    bool Input::IsMouseButtonPressed(MouseCode button)
    {
        return true;
    }
    Vec2 Input::GetMousePosition()
    {
        POINT ptMouse = {};
        GetCursorPos(&ptMouse);
        ScreenToClient(tomato::Application::Get().GetWindowHandle(), &ptMouse);

        return    { (float)ptMouse.x, (float)ptMouse.y };
    }
    void Input::SetMousePosition(const Vec2& position)
    {
        POINT ptMouse = { (LONG)position .x, (LONG)position .y};
        ClientToScreen(tomato::Application::Get().GetWindowHandle(), &ptMouse);
        SetCursorPos(ptMouse.x, ptMouse.y);
    }
    
}
