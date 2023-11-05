#pragma once

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Core/Window.h"

namespace Flux {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowCreateInfo& createInfo);
		virtual ~WindowsWindow();

		virtual void SetVisible(bool visible) const override;
		virtual bool IsVisible() const override;

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
	};
}

#endif