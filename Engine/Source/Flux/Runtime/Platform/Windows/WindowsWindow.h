#pragma once

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Runtime/Core/Window.h"

namespace Flux {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowCreateInfo& createInfo);
		~WindowsWindow();

		virtual WindowMenu CreateMenu() const override;
		virtual bool SetMenu(WindowMenu menu) const override;
		virtual bool AddMenu(WindowMenu menu, uint32 itemID = 0, const char* name = "") const override;
		virtual bool AddMenuSeparator(WindowMenu menu) const override;
		virtual bool AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name = "") const override;

		virtual void AddCloseCallback(const WindowCloseCallback& callback) { m_CloseCallbacks.push_back(callback); }
		virtual void AddSizeCallback(const WindowSizeCallback& callback) { m_SizeCallbacks.push_back(callback); }
		virtual void AddMenuCallback(const WindowMenuCallback& callback) { m_MenuCallbacks.push_back(callback); }

		virtual WindowHandle GetNativeHandle() const override { return m_WindowHandle; }

		int32 ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		HWND m_WindowHandle = NULL;

		uint32 m_Width = 0;
		uint32 m_Height = 0;
		std::string m_Title;

		std::vector<WindowCloseCallback> m_CloseCallbacks;
		std::vector<WindowSizeCallback> m_SizeCallbacks;
		std::vector<WindowMenuCallback> m_MenuCallbacks;

		WindowMenu m_Menu = nullptr;
	};

}

#endif