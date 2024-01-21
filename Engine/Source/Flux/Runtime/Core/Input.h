#pragma once

#include "KeyCodes.h"
#include "MouseButtonCodes.h"

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#define FLUX_ACTION_RELEASE 0
#define FLUX_ACTION_PRESS 1
#define FLUX_ACTION_REPEAT 2

namespace Flux {

	enum class KeyState : uint8
	{
		None = 0,
		Pressed,
		Released,
		Repeated
	};

	enum class MouseButtonState : uint8
	{
		None = 0,
		Pressed,
		Released
	};

	class Input
	{
	public:
		static void Init();
		static void Shutdown();

		static void Update();

		static bool GetKey(KeyCode key);
		static bool GetKeyDown(KeyCode key);
		static bool GetKeyUp(KeyCode key);

		static bool GetMouseButton(MouseButtonCode button);
		static bool GetMouseButtonDown(MouseButtonCode button);
		static bool GetMouseButtonUp(MouseButtonCode button);
	private:
		static void OnKeyPressedEvent(KeyPressedEvent& e);
		static void OnMouseMovedEvent(MouseMovedEvent& e);

		static void OnKeyEvent(int32 key, int32 scancode, int32 action, int32 mods);
		static void OnMouseButtonEvent(int32 button, int32 action, int32 mods);
		static void OnMouseMoveEvent(float x, float y);
	};

}