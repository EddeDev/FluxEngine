#include "FluxPCH.h"
#include "Input.h"

#include "Engine.h"

namespace Flux {

	struct InputData
	{
		KeyState* KeyStates;
		MouseButtonState* MouseButtonStates;

		int8 Keys[FLUX_KEY_LAST + 1];
		int8 MouseButtons[FLUX_MOUSE_BUTTON_LAST + 1];

		glm::vec2 MousePosition;
		glm::vec2 LastMousePosition;
		glm::vec2 MousePositionDelta;
		glm::vec2 MouseScroll;
	};

	static InputData* s_Data = nullptr;

	void Input::Init()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		s_Data = new InputData;
		memset(s_Data, 0, sizeof(InputData));

		s_Data->KeyStates = new KeyState[static_cast<size_t>(KeyCode::Last)];
		s_Data->MouseButtonStates = new MouseButtonState[static_cast<size_t>(MouseButtonCode::ButtonLast)];

		Engine::Get().SubmitToEventThread([]()
		{
			auto& window = Engine::Get().GetWindow();
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
	}

	void Input::Shutdown()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		delete[] s_Data->KeyStates;
		delete[] s_Data->MouseButtonStates;

		delete s_Data;
		s_Data = nullptr;
	}

	void Input::Update()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		s_Data->MousePositionDelta = s_Data->MousePosition - s_Data->LastMousePosition;
		s_Data->LastMousePosition = s_Data->MousePosition;

		memset(s_Data->KeyStates, 0, static_cast<int32>(KeyCode::Last));
		memset(s_Data->MouseButtonStates, 0, static_cast<int32>(MouseButtonCode::ButtonLast));
	}

	void Input::OnKeyEvent(int32 key, int32 scancode, int32 action, int32 mods)
	{
		FLUX_CHECK_IS_MAIN_THREAD();
		FLUX_ASSERT(key >= 0 || key == FLUX_KEY_UNKNOWN);
		FLUX_ASSERT(key <= FLUX_KEY_LAST);
		FLUX_ASSERT(action == FLUX_ACTION_PRESS || action == FLUX_ACTION_RELEASE);
		FLUX_ASSERT(mods == (mods & FLUX_MOD_MASK));

		if (key >= 0 && key <= FLUX_KEY_LAST)
		{
			if (action == FLUX_ACTION_RELEASE && s_Data->Keys[key] == FLUX_ACTION_RELEASE)
				return;

			const bool repeated = action == FLUX_ACTION_PRESS && s_Data->Keys[key] == FLUX_ACTION_PRESS;

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
		FLUX_CHECK_IS_MAIN_THREAD();
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
		FLUX_CHECK_IS_MAIN_THREAD();
		FLUX_ASSERT(x > -std::numeric_limits<float>::max());
		FLUX_ASSERT(x < std::numeric_limits<float>::max());
		FLUX_ASSERT(y > -std::numeric_limits<float>::max());
		FLUX_ASSERT(y < std::numeric_limits<float>::max());

		s_Data->MousePosition.x = x;
		s_Data->MousePosition.y = y;
	}

	bool Input::GetKey(KeyCode key)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		if (key < KeyCode::Space || key > KeyCode::Last)
		{
			FLUX_ERROR("Invalid key {0}", static_cast<int32>(key));
			return false;
		}
		
		return s_Data->Keys[static_cast<int32>(key)] == FLUX_ACTION_PRESS;
	}

	bool Input::GetKeyDown(KeyCode key)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		if (key < KeyCode::Space || key > KeyCode::Last)
		{
			FLUX_ERROR("Invalid key {0}", static_cast<int32>(key));
			return false;
		}

		return s_Data->KeyStates[static_cast<int32>(key)] == KeyState::Pressed;
	}

	bool Input::GetKeyUp(KeyCode key)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		if (key < KeyCode::Space || key > KeyCode::Last)
		{
			FLUX_ERROR("Invalid key {0}", static_cast<int32>(key));
			return false;
		}

		return s_Data->KeyStates[static_cast<int32>(key)] == KeyState::Released;
	}

	bool Input::GetMouseButton(MouseButtonCode button)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		if (button < MouseButtonCode::Button1 || button > MouseButtonCode::ButtonLast)
		{
			FLUX_ERROR("Invalid mouse button {0}", static_cast<int32>(button));
			return false;
		}
	
		return s_Data->MouseButtons[static_cast<int32>(button)] == FLUX_ACTION_PRESS;
	}

	bool Input::GetMouseButtonDown(MouseButtonCode button)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		if (button < MouseButtonCode::Button1 || button > MouseButtonCode::ButtonLast)
		{
			FLUX_ERROR("Invalid mouse button {0}", static_cast<int32>(button));
			return false;
		}

		return s_Data->MouseButtonStates[static_cast<int32>(button)] == MouseButtonState::Pressed;
	}

	bool Input::GetMouseButtonUp(MouseButtonCode button)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		if (button < MouseButtonCode::Button1 || button > MouseButtonCode::ButtonLast)
		{
			FLUX_ERROR("Invalid mouse button {0}", static_cast<int32>(button));
			return false;
		}

		return s_Data->MouseButtonStates[static_cast<int32>(button)] == MouseButtonState::Released;
	}

	glm::vec2 Input::GetMousePosition()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		return s_Data->LastMousePosition;
	}

	glm::vec2 Input::GetMousePositionDelta()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		return s_Data->MousePositionDelta;
	}

	glm::vec2 Input::GetMouseOrthoPosition(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		auto& window = Engine::Get().GetWindow();

		glm::vec4 result = { s_Data->MousePosition, 0.0f, 1.0f };
		result.x = (result.x / (float)window->GetWidth()) * 2.0f - 1.0f;
		result.y = (result.y / (float)window->GetHeight()) * 2.0f - 1.0f;
		result.y = -result.y;
		result = glm::inverse(viewMatrix) * glm::inverse(projectionMatrix) * result;
		return result;
	}

	glm::vec2 Input::GetMouseScroll()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		return s_Data->MouseScroll;
	}

}
