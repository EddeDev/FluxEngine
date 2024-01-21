#pragma once

#include "Event.h"

namespace Flux {

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_X(x), m_Y(y) {}

		float GetX() const { return m_X; }
		float GetY() const { return m_Y; }

		EVENT_CLASS_TYPE(MouseMoved)
	private:
		float m_X, m_Y;
	};

}