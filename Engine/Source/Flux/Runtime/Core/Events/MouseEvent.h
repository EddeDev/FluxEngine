#pragma once

#include "Event.h"

#include "Flux/Runtime/Core/Window.h"
#include "Flux/Runtime/Core/MouseButtonCodes.h"

namespace Flux {

	class MouseEvent : public Event
	{
	protected:
		MouseEvent(Ref<Window> window)
			: m_Window(window) {}
	public:
		Ref<Window> GetWindow() const { return m_Window; }
	private:
		Ref<Window> m_Window;
	};

	class MouseButtonPressedEvent : public MouseEvent
	{
	public:
		MouseButtonPressedEvent(Ref<Window> window, MouseButtonCode button)
			: MouseEvent(window), m_Button(button) {}

		MouseButtonCode GetButton() const { return m_Button; }

		EVENT_CLASS_TYPE(MouseButtonPressed)
	private:
		MouseButtonCode m_Button;
	};

	class MouseButtonReleasedEvent : public MouseEvent
	{
	public:
		MouseButtonReleasedEvent(Ref<Window> window, MouseButtonCode button)
			: MouseEvent(window), m_Button(button) {}

		MouseButtonCode GetButton() const { return m_Button; }

		EVENT_CLASS_TYPE(MouseButtonReleased)
	private:
		MouseButtonCode m_Button;
	};

	class MouseMovedEvent : public MouseEvent
	{
	public:
		MouseMovedEvent(Ref<Window> window, float x, float y)
			: MouseEvent(window), m_X(x), m_Y(y) {}

		float GetX() const { return m_X; }
		float GetY() const { return m_Y; }

		EVENT_CLASS_TYPE(MouseMoved)
	private:
		float m_X, m_Y;
	};

	class MouseScrolledEvent : public MouseEvent
	{
	public:
		MouseScrolledEvent(Ref<Window> window, float x, float y)
			: MouseEvent(window), m_X(x), m_Y(y) {}

		float GetX() const { return m_X; }
		float GetY() const { return m_Y; }

		EVENT_CLASS_TYPE(MouseScrolled)
	private:
		float m_X, m_Y;
	};

}