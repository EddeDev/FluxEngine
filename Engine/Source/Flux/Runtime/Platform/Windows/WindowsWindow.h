#pragma once

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Runtime/Core/Window.h"

namespace Flux {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowCreateInfo& createInfo);
		virtual ~WindowsWindow();

		virtual void SetSize(uint32 width, uint32 height) override;
		virtual std::pair<uint32, uint32> GetSize() const override { return { m_Width, m_Height }; }

		virtual void SetPosition(uint32 x, uint32 y) override;
		virtual std::pair<uint32, uint32> GetPosition() const override;

		virtual void SetTitle(const std::string& title) override;
		virtual const std::string& GetTitle() override;

		virtual void SetVisible(bool visible) const override;
		virtual bool IsVisible() const override;

		virtual void SetFocus() override;
		virtual bool IsFocused() const override;

		virtual WindowMenu CreateMenu() const override;
		virtual bool SetMenu(WindowMenu menu) const override;
		virtual bool AddMenu(WindowMenu menu, uint32 menuID = 0, const char* name = "", bool disabled = false) const override;
		virtual bool AddMenuSeparator(WindowMenu menu) const override;
		virtual bool AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name = "", bool disabled = false) const override;

		virtual void SetCursorShape(CursorShape shape);
		virtual CursorShape GetCursorShape() const override { return m_CursorShape; }

		virtual void SetEventQueue(Ref<EventQueue> eventQueue) override;
		virtual Ref<EventQueue> GetEventQueue() const override { return m_EventQueue; }

		virtual uint32 GetWidth() const override { return m_Width; }
		virtual uint32 GetHeight() const override { return m_Height; }

		virtual WindowHandle GetNativeHandle() const override { return m_WindowHandle; }

		int32 ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		template<typename T, typename... TArgs>
		void SubmitEvent(TArgs&&... args)
		{
			static_assert(std::is_base_of<Event, T>::value);
			if (m_EventQueue)
				m_EventQueue->AddEvent<T>(std::forward<TArgs>(args)...);
		}
	private:
		ThreadID m_ThreadID = 0;

		std::atomic<HWND> m_WindowHandle = NULL;

		std::atomic<uint32> m_Width = 0;
		std::atomic<uint32> m_Height = 0;

		std::atomic<uint32> m_PositionX = 0;
		std::atomic<uint32> m_PositionY = 0;

		DWORD m_Style = 0;
		DWORD m_ExStyle = 0;

		std::string m_Title;
		std::mutex m_TitleMutex;

		std::unordered_map<CursorShape, HCURSOR> m_CursorImageMap;
		std::atomic<CursorShape> m_CursorShape = CursorShape::Arrow;

		Ref<EventQueue> m_EventQueue;

		bool m_MouseButtons[FLUX_MOUSE_BUTTON_LAST + 1] = {};

		WCHAR m_HighSurrogate = 0;

		bool m_Minimized = false;
		bool m_Maximized = false;

		WindowMenu m_Menu = nullptr;
	};
}

#endif