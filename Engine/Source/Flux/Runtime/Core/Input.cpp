#include "FluxPCH.h"
#include "Input.h"

#include "Engine.h"

namespace Flux {

	struct InputData
	{
		KeyState* KeyStates;
		MouseButtonState* MouseButtonStates;

		InputAction Keys[FLUX_KEY_LAST + 1];
		InputAction MouseButtons[FLUX_MOUSE_BUTTON_LAST + 1];

		float MouseX;
		float MouseY;
		float PreviousMouseX;
		float PreviousMouseY;
		float MouseDeltaX;
		float MouseDeltaY;

		float MouseScrollX;
		float MouseScrollY;

		bool IsFirstFrame;
	};

	static InputData* s_Data = nullptr;

	void Input::Init()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		s_Data = new InputData();

		s_Data->KeyStates = new KeyState[static_cast<size_t>(KeyCode::Last)];
		s_Data->MouseButtonStates = new MouseButtonState[static_cast<size_t>(MouseButtonCode::ButtonLast)];

		s_Data->IsFirstFrame = true;
	}

	void Input::Shutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		delete[] s_Data->KeyStates;
		delete[] s_Data->MouseButtonStates;

		delete s_Data;
		s_Data = nullptr;
	}

	void Input::OnUpdate()
	{
		memset(s_Data->KeyStates, 0, static_cast<int32>(KeyCode::Last));
		memset(s_Data->MouseButtonStates, 0, static_cast<int32>(MouseButtonCode::ButtonLast));

		if (s_Data->IsFirstFrame)
		{
			s_Data->PreviousMouseX = s_Data->MouseX;
			s_Data->PreviousMouseY = s_Data->MouseY;

			s_Data->IsFirstFrame = false;
		}

		s_Data->MouseDeltaX = s_Data->MouseX - s_Data->PreviousMouseX;
		s_Data->MouseDeltaY = s_Data->MouseY - s_Data->PreviousMouseY;
		s_Data->PreviousMouseX = s_Data->MouseX;
		s_Data->PreviousMouseY = s_Data->MouseY;

		s_Data->MouseScrollX = 0.0f;
		s_Data->MouseScrollY = 0.0f;
	}

	bool Input::GetKey(KeyCode key)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (key < KeyCode::Space || key > KeyCode::Last)
		{
			FLUX_ERROR("Invalid key {0}", (int32)key);
			return false;
		}

		return s_Data->Keys[(int32)key] == InputAction::Press;
	}

	bool Input::GetKeyDown(KeyCode key)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (key < KeyCode::Space || key > KeyCode::Last)
		{
			FLUX_ERROR("Invalid key {0}", (int32)key);
			return false;
		}

		return s_Data->KeyStates[(int32)key] == KeyState::Pressed;
	}

	bool Input::GetKeyUp(KeyCode key)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (key < KeyCode::Space || key > KeyCode::Last)
		{
			FLUX_ERROR("Invalid key {0}", (int32)key);
			return false;
		}

		return s_Data->KeyStates[(int32)key] == KeyState::Released;
	}

	bool Input::GetMouseButton(MouseButtonCode button)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (button < MouseButtonCode::Button1 || button > MouseButtonCode::ButtonLast)
		{
			FLUX_ERROR("Invalid mouse button {0}", (int32)button);
			return false;
		}

		return s_Data->MouseButtons[(int32)button] == InputAction::Press;
	}

	bool Input::GetMouseButtonDown(MouseButtonCode button)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (button < MouseButtonCode::Button1 || button > MouseButtonCode::ButtonLast)
		{
			FLUX_ERROR("Invalid mouse button {0}", static_cast<int32>(button));
			return false;
		}

		return s_Data->MouseButtonStates[static_cast<int32>(button)] == MouseButtonState::Pressed;
	}

	bool Input::GetMouseButtonUp(MouseButtonCode button)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (button < MouseButtonCode::Button1 || button > MouseButtonCode::ButtonLast)
		{
			FLUX_ERROR("Invalid mouse button {0}", static_cast<int32>(button));
			return false;
		}

		return s_Data->MouseButtonStates[static_cast<int32>(button)] == MouseButtonState::Released;
	}

	float Input::GetAxis(std::string_view name)
	{
		// TODO: Input manager

		if (name == "Horizontal")
		{
			float left = GetKey(KeyCode::A) ? -1.0f : 0.0f;
			float right = GetKey(KeyCode::D) ? 1.0f : 0.0f;

			return left + right;
		}

		if (name == "Vertical")
		{
			float up = GetKey(KeyCode::W) ? 1.0f : 0.0f;
			float down = GetKey(KeyCode::S) ? -1.0f : 0.0f;

			return up + down;
		}

		if (name == "Mouse X")
			return s_Data->MouseDeltaX;

		if (name == "Mouse Y")
			return s_Data->MouseDeltaY;

		return 0.0f;
	}

	float Input::GetMouseScrollDeltaX()
	{
		return s_Data->MouseScrollX;
	}

	float Input::GetMouseScrollDeltaY()
	{
		return s_Data->MouseScrollY;
	}

	void Input::OnEvent(Event& event)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		EventHandler handler(event);
		handler.Bind<KeyPressedEvent>(FLUX_BIND_CALLBACK(OnKeyPressedEvent));
		handler.Bind<KeyReleasedEvent>(FLUX_BIND_CALLBACK(OnKeyReleasedEvent));
		handler.Bind<MouseMovedEvent>(FLUX_BIND_CALLBACK(OnMouseMovedEvent));
		handler.Bind<MouseButtonPressedEvent>(FLUX_BIND_CALLBACK(OnMouseButtonPressedEvent));
		handler.Bind<MouseButtonReleasedEvent>(FLUX_BIND_CALLBACK(OnMouseButtonReleasedEvent));
		handler.Bind<MouseScrolledEvent>(FLUX_BIND_CALLBACK(OnMouseScrolledEvent));
	}

	void Input::OnKeyPressedEvent(KeyPressedEvent& event)
	{
		int32 key = (int32)event.GetKey();

		KeyState state = s_Data->Keys[key] == InputAction::Press ? KeyState::Repeated : KeyState::Pressed;
		s_Data->Keys[key] = InputAction::Press;
		s_Data->KeyStates[key] = state;
	}

	void Input::OnKeyReleasedEvent(KeyReleasedEvent& event)
	{
		int32 key = (int32)event.GetKey();

		s_Data->Keys[key] = InputAction::Release;
		s_Data->KeyStates[key] = KeyState::Released;
	}

	void Input::OnMouseMovedEvent(MouseMovedEvent& event)
	{
		s_Data->MouseX = event.GetX();
		s_Data->MouseY = event.GetY();
	}

	void Input::OnMouseButtonPressedEvent(MouseButtonPressedEvent& event)
	{
		int32 button = (int32)event.GetButton();

		s_Data->MouseButtons[button] = InputAction::Press;
		s_Data->MouseButtonStates[button] = MouseButtonState::Pressed;
	}

	void Input::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event)
	{
		int32 button = (int32)event.GetButton();

		s_Data->MouseButtons[button] = InputAction::Release;
		s_Data->MouseButtonStates[button] = MouseButtonState::Released;
	}

	void Input::OnMouseScrolledEvent(MouseScrolledEvent& event)
	{
		s_Data->MouseScrollX = event.GetX();
		s_Data->MouseScrollY = event.GetY();
	}

}