#pragma once

#include "Event.h"

#include "Flux/Runtime/Core/Window.h"
#include "Flux/Runtime/Core/KeyCodes.h"

namespace Flux {

	class KeyEvent : public Event
	{
	protected:
		KeyEvent(Ref<Window> window, KeyCode key)
			: m_KeyCode(key) {}
	public:
		Ref<Window> GetWindow() const { return m_Window; }
		KeyCode GetKey() const { return m_KeyCode; }
	private:
		Ref<Window> m_Window;
		KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(Ref<Window> window, KeyCode key)
			: KeyEvent(window, key) {}

		EVENT_CLASS_TYPE(KeyPressed)
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(Ref<Window> window, KeyCode key)
			: KeyEvent(window, key) {}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public Event
	{
	public:
		KeyTypedEvent(Ref<Window> window, char32 codepoint)
			:  m_CodePoint(codepoint) {}

		Ref<Window> GetWindow() const { return m_Window; }
		char32 GetCodePoint() const { return m_CodePoint; }

		EVENT_CLASS_TYPE(KeyTyped)
	private:
		Ref<Window> m_Window;
		char32 m_CodePoint;
	};

}