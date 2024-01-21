#pragma once

#include "Flux/Runtime/Core/BaseTypes.h"

namespace Flux {

	enum class EventType : uint8
	{
		None = 0,

		WindowClose, WindowResize, WindowFocus, WindowMenu,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	namespace Utils {

		static const char* EventTypeToString(EventType type)
		{
			switch (type)
			{
			case EventType::WindowClose:         return "WindowClose";
			case EventType::WindowResize:        return "WindowResize";
			case EventType::WindowFocus:         return "WindowFocus";
			case EventType::WindowMenu:          return "WindowMenu";
			case EventType::KeyPressed:          return "KeyPressed";
			case EventType::KeyReleased:         return "KeyReleased";
			case EventType::KeyTyped:            return "KeyTyped";
			case EventType::MouseButtonPressed:  return "MouseButtonPressed";
			case EventType::MouseButtonReleased: return "MouseButtonReleased";
			case EventType::MouseMoved:          return "MouseMoved";
			case EventType::MouseScrolled:       return "MouseScrolled";
			}
			return "Unknown";
		}

	}

}