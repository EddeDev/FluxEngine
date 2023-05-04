#pragma once

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Core/Window/Window.h"

namespace Flux {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowCreateInfo& createInfo);
		~WindowsWindow();

		virtual void ProcessEvents() const override;

		virtual void AddCloseCallback(const WindowCloseCallback& callback) { m_Data.CloseCallbacks.push_back(callback); }
		virtual void AddSizeCallback(const WindowSizeCallback& callback) { m_Data.SizeCallbacks.push_back(callback); }
	private:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		HWND m_WindowHandle = NULL;

		struct WindowData
		{
			uint32 Width;
			uint32 Height;
			std::string Title;

			std::vector<WindowCloseCallback> CloseCallbacks;
			std::vector<WindowSizeCallback> SizeCallbacks;
		} m_Data;

		inline static ATOM s_WindowClass = 0;
		inline static uint32 s_WindowCount = 0;
	};

}

#endif