#pragma once

#include "Event.h"

#include "Flux/Runtime/Core/Window.h"

namespace Flux {

	class WindowEvent : public Event
	{
	public:
		WindowEvent(Ref<Window> window)
			: m_Window(window) {}

		Ref<Window> GetWindow() const { return m_Window; }
	private:
		Ref<Window> m_Window;
	};

	class WindowCloseEvent : public WindowEvent
	{
	public:
		WindowCloseEvent(Ref<Window> window)
			: WindowEvent(window) {}

		EVENT_CLASS_TYPE(WindowClose)
	};

	class WindowResizeEvent : public WindowEvent
	{
	public:
		WindowResizeEvent(Ref<Window> window, uint32 width, uint32 height)
			: WindowEvent(window), m_Width(width), m_Height(height) {}

		uint32 GetWidth() const { return m_Width; }
		uint32 GetHeight() const { return m_Height; }

		EVENT_CLASS_TYPE(WindowResize)
	private:
		uint32 m_Width;
		uint32 m_Height;
	};

	class WindowMaximizeEvent : public WindowEvent
	{
	public:
		WindowMaximizeEvent(Ref<Window> window, bool maximized)
			: WindowEvent(window), m_Maximized(maximized) {}

		bool IsMaximized() const { return m_Maximized; }

		EVENT_CLASS_TYPE(WindowMaximize)
	private:
		bool m_Maximized;
	};

	class WindowMinimizeEvent : public WindowEvent
	{
	public:
		WindowMinimizeEvent(Ref<Window> window, bool minimized)
			: WindowEvent(window), m_Minimized(minimized) {}

		bool IsMinimized() const { return m_Minimized; }

		EVENT_CLASS_TYPE(WindowMinimize)
	private:
		bool m_Minimized;
	};

	class WindowFocusEvent : public WindowEvent
	{
	public:
		WindowFocusEvent(Ref<Window> window, bool focused)
			: WindowEvent(window), m_Focused(focused) {}

		bool IsFocused() const { return m_Focused; }

		EVENT_CLASS_TYPE(WindowFocus)
	private:
		bool m_Focused;
	};

	class WindowMenuEvent : public WindowEvent
	{
	public:
		WindowMenuEvent(Ref<Window> window, WindowMenu menu, uint32 itemID)
			: WindowEvent(window), m_Menu(menu), m_ItemID(itemID) {}

		WindowMenu GetMenu() const { return m_Menu; }
		uint32 GetItemID() const { return m_ItemID; }

		EVENT_CLASS_TYPE(WindowMenu)
	private:
		WindowMenu m_Menu;
		uint32 m_ItemID;
	};

}