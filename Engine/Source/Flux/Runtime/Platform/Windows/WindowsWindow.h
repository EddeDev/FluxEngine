#pragma once

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Runtime/Core/Window.h"

namespace Flux {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowCreateInfo& createInfo);
		virtual ~WindowsWindow();

		virtual void SetVisible(bool visible) const override;
		virtual bool IsVisible() const override;

		virtual WindowMenu CreateMenu() const override;
		virtual bool SetMenu(WindowMenu menu) const override;
		virtual bool AddMenu(WindowMenu menu, uint32 menuID = 0, const char* name = "", bool disabled = false) const override;
		virtual bool AddMenuSeparator(WindowMenu menu) const override;
		virtual bool AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name = "", bool disabled = false) const override;

		virtual void SetCursorShape(CursorShape shape);
		virtual CursorShape GetCursorShape() const override { return m_CursorShape; }

		virtual void SetEventQueue(Ref<EventQueue> eventQueue) override;

		virtual uint32 GetWidth() const override { return m_Width; }
		virtual uint32 GetHeight() const override { return m_Height; }

		virtual WindowHandle GetNativeHandle() const override { return m_WindowHandle; }

		int32 ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		ThreadID m_ThreadID = 0;

		std::atomic<HWND> m_WindowHandle = NULL;

		std::atomic<uint32> m_Width = 0;
		std::atomic<uint32> m_Height = 0;
		std::string m_Title;

		std::unordered_map<CursorShape, HCURSOR> m_CursorImageMap;
		std::atomic<CursorShape> m_CursorShape = CursorShape::Arrow;

		Ref<EventQueue> m_EventQueue;

		WCHAR m_HighSurrogate = 0;

		WindowMenu m_Menu = nullptr;
	};
}

#endif