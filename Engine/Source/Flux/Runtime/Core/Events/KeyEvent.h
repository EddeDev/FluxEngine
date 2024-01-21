#pragma once

#include "Event.h"

// TODO
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

}