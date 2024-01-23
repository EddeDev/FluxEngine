#pragma once

#include "Event.h"

#include "Flux/Runtime/Core/KeyCodes.h"

namespace Flux {

	class KeyEvent : public Event
	{
	public:
		KeyCode GetKey() const { return m_KeyCode; }
	protected:
		KeyEvent(KeyCode key)
			: m_KeyCode(key) {}
	private:
		KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyCode key)
			: KeyEvent(key) {}

		EVENT_CLASS_TYPE(KeyPressed)
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(KeyCode key)
			: KeyEvent(key) {}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public Event
	{
	public:
		KeyTypedEvent(char32 codepoint)
			:  m_CodePoint(codepoint) {}

		char32 GetCodePoint() const { return m_CodePoint; }

		EVENT_CLASS_TYPE(KeyTyped)
	private:
		char32 m_CodePoint;
	};

}