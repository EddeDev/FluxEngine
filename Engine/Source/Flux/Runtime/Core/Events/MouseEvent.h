#pragma once

#include "Event.h"

namespace Flux {

	class MouseButtonPressedEvent : public Event
	{
	public:
		MouseButtonPressedEvent(MouseButtonCode button)
			: m_Button(button) {}

		MouseButtonCode GetButton() const { return m_Button; }

		EVENT_CLASS_TYPE(MouseButtonPressed)
	private:
		MouseButtonCode m_Button;
	};

	class MouseButtonReleasedEvent : public Event
	{
	public:
		MouseButtonReleasedEvent(MouseButtonCode button)
			: m_Button(button) {}

		MouseButtonCode GetButton() const { return m_Button; }

		EVENT_CLASS_TYPE(MouseButtonReleased)
	private:
		MouseButtonCode m_Button;
	};

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

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float x, float y)
			: m_X(x), m_Y(y) {}

		float GetX() const { return m_X; }
		float GetY() const { return m_Y; }

		EVENT_CLASS_TYPE(MouseScrolled)
	private:
		float m_X, m_Y;
	};

}