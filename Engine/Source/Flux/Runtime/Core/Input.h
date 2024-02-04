#pragma once

#include "KeyCodes.h"
#include "MouseButtonCodes.h"

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

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

	enum class InputAction : uint8
	{
		None = 0,
		Press,
		Release
	};

	class Input
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnEvent(Event& event);
		static void OnUpdate();

		static bool GetKey(KeyCode key);
		static bool GetKeyDown(KeyCode key);
		static bool GetKeyUp(KeyCode key);

		static bool GetMouseButton(MouseButtonCode button);
		static bool GetMouseButtonDown(MouseButtonCode button);
		static bool GetMouseButtonUp(MouseButtonCode button);

		static float GetAxis(std::string_view name);

		static float GetMouseScrollDeltaX();
		static float GetMouseScrollDeltaY();
	private:
		static void OnKeyPressedEvent(KeyPressedEvent& event);
		static void OnKeyReleasedEvent(KeyReleasedEvent& event);
		static void OnMouseMovedEvent(MouseMovedEvent& event);
		static void OnMouseButtonPressedEvent(MouseButtonPressedEvent& event);
		static void OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event);
		static void OnMouseScrolledEvent(MouseScrolledEvent& event);
	};

}