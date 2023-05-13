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

		virtual void AddCloseCallback(const WindowCloseCallback& callback) override;
		virtual void AddSizeCallback(const WindowSizeCallback& callback) override;
		virtual void AddMenuCallback(const WindowMenuCallback& callback) override;

		virtual WindowHandle GetNativeHandle() const override { return m_WindowHandle; }

		int32 ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		HWND m_WindowHandle = NULL;

		uint32 m_Width = 0;
		uint32 m_Height = 0;
		std::string m_Title;
		
		ThreadID m_ThreadID;

		std::vector<WindowCloseCallback> m_CloseCallbacks;
		std::vector<WindowSizeCallback> m_SizeCallbacks;
		std::vector<WindowMenuCallback> m_MenuCallbacks;

		WindowMenu m_Menu = nullptr;
	};

}

#endif