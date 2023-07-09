#pragma once

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Runtime/Core/Window.h"

namespace Flux {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowCreateInfo& createInfo);
		virtual ~WindowsWindow();

		virtual WindowMenu CreateMenu() const override;
		virtual bool SetMenu(WindowMenu menu) const override;
		virtual bool AddMenu(WindowMenu menu, uint32 itemID = 0, const char* name = "", bool disabled = false) const override;
		virtual bool AddMenuSeparator(WindowMenu menu) const override;
		virtual bool AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name = "", bool disabled = false) const override;

		virtual void SetVisible(bool visible) const override;
		virtual bool IsVisible() const override;

		virtual void AddCloseCallback(const WindowCloseCallback& callback) override;
		virtual void AddMinimizeCallback(const WindowMinimizeCallback& callback) override;
		virtual void AddSizeCallback(const WindowSizeCallback& callback) override;
		virtual void AddMenuCallback(const WindowMenuCallback& callback) override;
		virtual void AddDropCallback(const WindowDropCallback& callback) override;
		virtual void AddKeyCallback(const KeyCallback& callback) override;
		virtual void AddMouseButtonCallback(const MouseButtonCallback& callback) override;
		virtual void AddMouseMoveCallback(const MouseMoveCallback& callback) override;

		virtual uint32 GetWidth() const override { return m_Width; }
		virtual uint32 GetHeight() const override { return m_Height; }

		virtual WindowHandle GetNativeHandle() const override { return m_WindowHandle; }

		int32 ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		std::atomic<HWND> m_WindowHandle = NULL;

		std::atomic<uint32> m_Width = 0;
		std::atomic<uint32> m_Height = 0;

		std::string m_Title;

		std::atomic<bool> m_Minimized = false;
		
		ThreadID m_ThreadID = 0;

		std::vector<WindowCloseCallback> m_CloseCallbacks;
		std::vector<WindowMinimizeCallback> m_MinimizeCallbacks;
		std::vector<WindowSizeCallback> m_SizeCallbacks;
		std::vector<WindowMenuCallback> m_MenuCallbacks;
		std::vector<WindowDropCallback> m_DropCallbacks;
		std::vector<KeyCallback> m_KeyCallbacks;
		std::vector<MouseButtonCallback> m_MouseButtonCallbacks;
		std::vector<MouseMoveCallback> m_MouseMoveCallbacks;

		bool m_CursorTracked = false;

		WindowMenu m_Menu = nullptr;
	};

}

#endif