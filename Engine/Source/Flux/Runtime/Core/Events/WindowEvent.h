#pragma once

#include "Event.h"

#include "Flux/Runtime/Core/Window.h"

namespace Flux {

	class WindowCloseEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowClose)
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32 width, uint32 height)
			: m_Width(width), m_Height(height) {}

		uint32 GetWidth() const { return m_Width; }
		uint32 GetHeight() const { return m_Height; }

		EVENT_CLASS_TYPE(WindowResize)
	private:
		uint32 m_Width;
		uint32 m_Height;
	};

	class WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent(bool focused)
			: m_Focused(focused) {}

		bool IsFocused() const { return m_Focused; }

		EVENT_CLASS_TYPE(WindowFocus)
	private:
		bool m_Focused;
	};

	class WindowMenuEvent : public Event
	{
	public:
		WindowMenuEvent(WindowMenu menu, uint32 itemID)
			: m_Menu(menu), m_ItemID(itemID) {}

		WindowMenu GetMenu() const { return m_Menu; }
		uint32 GetItemID() const { return m_ItemID; }

		EVENT_CLASS_TYPE(WindowMenu)
	private:
		WindowMenu m_Menu;
		uint32 m_ItemID;
	};

}