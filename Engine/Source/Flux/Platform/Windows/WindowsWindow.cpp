#include "FluxPCH.h"

#ifdef FLUX_PLATFORM_WINDOWS

#include "WindowsWindow.h"

#include <VersionHelpers.h>
#include <windowsx.h>

#ifndef WM_COPYGLOBALDATA
	#define WM_COPYGLOBALDATA 0x0049
#endif

namespace Flux {

	extern HINSTANCE g_Instance;

	WindowsWindow::WindowsWindow(const WindowCreateInfo& createInfo)
	{
		m_Width = createInfo.Width;
		m_Height = createInfo.Height;
		m_Title = createInfo.Title;

		m_ThreadID = Platform::GetCurrentThreadID();

		DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX;

		if (createInfo.Decorated)
		{
			style |= WS_CAPTION;

			if (createInfo.Resizable)
				style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
		}
		else
		{
			style |= WS_POPUP;
		}

		DWORD exStyle = 0;
		exStyle |= WS_EX_APPWINDOW;

		int32 windowX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (m_Width / 2);
		int32 windowY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (m_Height / 2);

		RECT rect = { 0, 0, static_cast<LONG>(m_Width), static_cast<LONG>(m_Height) };
		if (AdjustWindowRectEx(&rect, style, FALSE, exStyle))
		{
			windowX = windowX + rect.left;
			windowY = windowY + rect.top;
			m_Width = rect.right - rect.left;
			m_Height = rect.bottom - rect.top;
		}

		ATOM windowClass = static_cast<ATOM>(Platform::GetWindowClass());
		if (!windowClass)
		{
			FLUX_ASSERT(false, "Window class is NULL!");
			return;
		}

		wchar_t* title = new wchar_t[createInfo.Title.size() + 1];
		MultiByteToWideChar(CP_UTF8, 0, createInfo.Title.c_str(), -1, title, static_cast<int32>(createInfo.Title.size()) + 1);

		m_WindowHandle = CreateWindowExW(
			exStyle,
			MAKEINTATOM(windowClass),
			title,
			style,
			windowX,
			windowY,
			m_Width,
			m_Height,
			NULL,
			NULL,
			g_Instance,
			NULL
		);

		delete[] title;

		if (!m_WindowHandle)
		{
			FLUX_ASSERT(false, "Failed to create window. ({0})", Platform::GetErrorMessage());
			return;
		}

		SetPropW(m_WindowHandle, L"Window", this);

		DragAcceptFiles(m_WindowHandle, TRUE);

		RECT clientRect;
		if (GetClientRect(m_WindowHandle, &clientRect))
		{
			m_Width = clientRect.right;
			m_Height = clientRect.bottom;
		}
	}

	WindowsWindow::~WindowsWindow()
	{
		FLUX_CHECK_IS_THREAD(m_ThreadID);

		RemovePropW(m_WindowHandle, L"Window");
		DestroyWindow(m_WindowHandle);
	}

	void WindowsWindow::SetVisible(bool visible) const
	{
		FLUX_CHECK_IS_THREAD(m_ThreadID);

		::ShowWindow(m_WindowHandle, visible ? SW_SHOWNA : SW_HIDE);
	}

	bool WindowsWindow::IsVisible() const
	{
		FLUX_CHECK_IS_THREAD(m_ThreadID);

		return ::IsWindowVisible(m_WindowHandle);
	}

	void WindowsWindow::AddCloseCallback(const WindowCloseCallback& callback)
	{
		FLUX_CHECK_IS_THREAD(m_ThreadID);

		m_CloseCallbacks.push_back(callback);
	}

	int32 WindowsWindow::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		FLUX_CHECK_IS_THREAD(m_ThreadID);

		switch (uMsg)
		{
		case WM_CLOSE:
		{
			for (auto& callback : m_CloseCallbacks)
				callback();
			return 0;
		}
		}

		return static_cast<int32>(DefWindowProcW(m_WindowHandle, uMsg, wParam, lParam));
	}

}

#endif