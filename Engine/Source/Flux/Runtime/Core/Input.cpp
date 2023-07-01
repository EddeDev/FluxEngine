#include "FluxPCH.h"
#include "Input.h"

#include "Engine.h"

namespace Flux {

	struct InputData
	{
		KeyState* KeyStates;
		MouseButtonState* MouseButtonStates;

		glm::vec2 MousePosition;
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

			window->AddKeyPressCallback([](auto key)
			{
				Engine::Get().SubmitToMainThread([key]()
				{
					s_Data->KeyStates[key] = KeyState::Pressed;
				});
			});

			window->AddKeyReleaseCallback([](auto key)
			{
				Engine::Get().SubmitToMainThread([key]()
				{
					s_Data->KeyStates[key] = KeyState::Released;
				});
			});

			window->AddKeyRepeatCallback([](auto key)
			{
				Engine::Get().SubmitToMainThread([key]()
				{
					s_Data->KeyStates[key] = KeyState::Repeated;
				});
			});

			window->AddMouseButtonPressCallback([](auto button)
			{
				Engine::Get().SubmitToMainThread([button]()
				{
					s_Data->MouseButtonStates[button] = MouseButtonState::Pressed;
				});
			});

			window->AddMouseButtonReleaseCallback([](auto button)
			{
				Engine::Get().SubmitToMainThread([button]()
				{
					s_Data->MouseButtonStates[button] = MouseButtonState::Released;
				});
			});

			window->AddScrollCallback([](auto xoffset, auto yoffset)
			{
				Engine::Get().SubmitToMainThread([xoffset, yoffset]()
				{
					s_Data->MouseScroll = { (float)xoffset, (float)yoffset };
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

		auto& window = Engine::Get().GetWindow();
		if (window)
		{
			glm::vec2 mousePosition = window->GetMousePosition();
			mousePosition.y = static_cast<float>(window->GetHeight()) - mousePosition.y;
			s_Data->MousePositionDelta = s_Data->MousePosition - mousePosition;
			s_Data->MousePosition = mousePosition;
			s_Data->MouseScroll = {};
		}

		memset(s_Data->KeyStates, 0, static_cast<int32>(KeyCode::Last));
		memset(s_Data->MouseButtonStates, 0, static_cast<int32>(MouseButtonCode::ButtonLast));
	}

	bool Input::GetKey(KeyCode key)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		auto& window = Engine::Get().GetWindow();
		if (window)
			return window->IsKeyDown(static_cast<int32>(key));
		return false;
	}

	bool Input::GetKeyDown(KeyCode key)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		return s_Data->KeyStates[static_cast<int32>(key)] == KeyState::Pressed;
	}

	bool Input::GetKeyUp(KeyCode key)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		return s_Data->KeyStates[static_cast<int32>(key)] == KeyState::Released;
	}

	bool Input::GetMouseButton(MouseButtonCode button)
	{
		FLUX_CHECK_IS_MAIN_THREAD();
	
		auto& window = Engine::Get().GetWindow();
		if (window)
			return window->IsMouseButtonDown(static_cast<int32>(button));
		return false;
	}

	bool Input::GetMouseButtonDown(MouseButtonCode button)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		return s_Data->MouseButtonStates[static_cast<int32>(button)] == MouseButtonState::Pressed;
	}

	bool Input::GetMouseButtonUp(MouseButtonCode button)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		return s_Data->MouseButtonStates[static_cast<int32>(button)] == MouseButtonState::Released;
	}

}
