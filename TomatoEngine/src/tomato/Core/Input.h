#pragma once

#include "tomato/Core/KeyCodes.h"
#include "tomato/Core/MouseCodes.h"

namespace tomato
{
	class Input
	{
	public:
		[[nodiscard]] static bool IsKeyPressed(KeyCode key);
		
		[[nodiscard]] static bool IsMouseButtonPressed(MouseCode button);
		[[nodiscard]] static Vec2 GetMousePosition();
		static void SetMousePosition(const Vec2& position);
	};
}
