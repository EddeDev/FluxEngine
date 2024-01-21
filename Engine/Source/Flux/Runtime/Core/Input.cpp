#include "FluxPCH.h"
#include "Input.h"

#include "Engine.h"

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

namespace Flux {

	struct InputData
	{
		KeyState* KeyStates;
		MouseButtonState* MouseButtonStates;

		int8 Keys[FLUX_KEY_LAST + 1];
		int8 MouseButtons[FLUX_MOUSE_BUTTON_LAST + 1];

		EventCallbackID KeyPressedEventCallbackID;
		EventCallbackID MouseMovedEventCallbackID;
	};

	static InputData* s_Data = nullptr;

	void Input::Init()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		s_Data = new InputData;
		memset(s_Data, 0, sizeof(InputData));

		s_Data->KeyStates = new KeyState[static_cast<size_t>(KeyCode::Last)];
		s_Data->MouseButtonStates = new MouseButtonState[static_cast<size_t>(MouseButtonCode::ButtonLast)];

		s_Data->KeyPressedEventCallbackID = Engine::Get().GetMainWindow()->GetEventManager().Subscribe<KeyPressedEvent>(OnKeyPressedEvent);
		s_Data->MouseMovedEventCallbackID = Engine::Get().GetMainWindow()->GetEventManager().Subscribe<MouseMovedEvent>(OnMouseMovedEvent);

#if 0
		Engine::Get().SubmitToEventThread([]()
		{
			Ref<Window> window = Engine::Get().GetWindow();
			if (!window)
				return;

			window->AddKeyCallback([](auto key, auto scancode, auto action, auto mods)
			{
				Engine::Get().SubmitToMainThread([key, scancode, action, mods]()
				{
					Input::OnKeyEvent(key, scancode, action, mods);
				});
			});

			window->AddMouseButtonCallback([](auto button, auto action, auto mods)
			{
				Engine::Get().SubmitToMainThread([button, action, mods]()
				{
					Input::OnMouseButtonEvent(button, action, mods);
				});
			});

			window->AddMouseMoveCallback([](auto x, auto y)
			{
				Engine::Get().SubmitToMainThread([x, y]()
				{
					Input::OnMouseMoveEvent(x, y);
				});
			});
		});
#endif
	}

	void Input::Shutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		Engine::Get().GetMainWindow()->GetEventManager().Unsubscribe(s_Data->KeyPressedEventCallbackID);
		Engine::Get().GetMainWindow()->GetEventManager().Unsubscribe(s_Data->MouseMovedEventCallbackID);

		delete[] s_Data->KeyStates;
		delete[] s_Data->MouseButtonStates;

		delete s_Data;
		s_Data = nullptr;
	}

	void Input::Update()
	{
		memset(s_Data->KeyStates, 0, static_cast<int32>(KeyCode::Last));
		memset(s_Data->MouseButtonStates, 0, static_cast<int32>(MouseButtonCode::ButtonLast));
	}

	bool Input::GetKey(KeyCode key)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (key < KeyCode::Space || key > KeyCode::Last)
		{
			FLUX_ERROR("Invalid key {0}", static_cast<int32>(key));
			return false;
		}

		return s_Data->Keys[static_cast<int32>(key)] == FLUX_ACTION_PRESS;
	}

	bool Input::GetKeyDown(KeyCode key)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (key < KeyCode::Space || key > KeyCode::Last)
		{
			FLUX_ERROR("Invalid key {0}", static_cast<int32>(key));
			return false;
		}

		return s_Data->KeyStates[static_cast<int32>(key)] == KeyState::Pressed;
	}

	bool Input::GetKeyUp(KeyCode key)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (key < KeyCode::Space || key > KeyCode::Last)
		{
			FLUX_ERROR("Invalid key {0}", static_cast<int32>(key));
			return false;
		}

		return s_Data->KeyStates[static_cast<int32>(key)] == KeyState::Released;
	}

	bool Input::GetMouseButton(MouseButtonCode button)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (button < MouseButtonCode::Button1 || button > MouseButtonCode::ButtonLast)
		{
			FLUX_ERROR("Invalid mouse button {0}", static_cast<int32>(button));
			return false;
		}

		return s_Data->MouseButtons[static_cast<int32>(button)] == FLUX_ACTION_PRESS;
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

	void Input::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();


		// s_Data->KeyStates[static_cast<int32>(e.GetKey())] = KeyState::Pressed;
	}

	void Input::OnMouseMovedEvent(MouseMovedEvent& e)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();
	}

	void Input::OnKeyEvent(int32 key, int32 scancode, int32 action, int32 mods)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_ASSERT(key >= 0 || key == FLUX_KEY_UNKNOWN);
		FLUX_ASSERT(key <= FLUX_KEY_LAST);
		FLUX_ASSERT(action == FLUX_ACTION_PRESS || action == FLUX_ACTION_RELEASE);
		FLUX_ASSERT(mods == (mods & FLUX_MOD_MASK));

		if (key >= 0 && key <= FLUX_KEY_LAST)
		{
			if (action == FLUX_ACTION_RELEASE && s_Data->Keys[key] == FLUX_ACTION_RELEASE)
				return;

			bool repeated = action == FLUX_ACTION_PRESS && s_Data->Keys[key] == FLUX_ACTION_PRESS;

			s_Data->Keys[key] = static_cast<int8>(action);

			if (repeated)
				action = FLUX_ACTION_REPEAT;
		}

		switch (action)
		{
		case FLUX_ACTION_PRESS:
		{
			s_Data->KeyStates[key] = KeyState::Pressed;
			break;
		}
		case FLUX_ACTION_RELEASE:
		{
			s_Data->KeyStates[key] = KeyState::Released;
			break;
		}
		case FLUX_ACTION_REPEAT:
		{
			s_Data->KeyStates[key] = KeyState::Repeated;
			break;
		}
		}
	}

	void Input::OnMouseButtonEvent(int32 button, int32 action, int32 mods)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_ASSERT(button >= 0);
		FLUX_ASSERT(button <= FLUX_MOUSE_BUTTON_LAST);
		FLUX_ASSERT(action == FLUX_ACTION_PRESS || action == FLUX_ACTION_RELEASE);
		FLUX_ASSERT(mods == (mods & FLUX_MOD_MASK));

		if (button < 0 || button > FLUX_MOUSE_BUTTON_LAST)
			return;

		s_Data->MouseButtons[button] = (int8)action;

		switch (action)
		{
		case FLUX_ACTION_PRESS:
		{
			s_Data->MouseButtonStates[button] = MouseButtonState::Pressed;
			break;
		}
		case FLUX_ACTION_RELEASE:
		{
			s_Data->MouseButtonStates[button] = MouseButtonState::Released;
			break;
		}
		}
	}

	void Input::OnMouseMoveEvent(float x, float y)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();
	}

}