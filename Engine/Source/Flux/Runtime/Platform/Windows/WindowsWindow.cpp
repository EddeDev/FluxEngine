#include "FluxPCH.h"
#include "WindowsWindow.h"

#ifdef FLUX_PLATFORM_WINDOWS

#include <VersionHelpers.h>

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

		DWORD style = 0;
		style |= WS_CLIPSIBLINGS;
		style |= WS_CLIPCHILDREN;
		style |= WS_SYSMENU;
		style |= WS_MINIMIZEBOX;
		style |= WS_CAPTION;
		style |= WS_MAXIMIZEBOX;
		style |= WS_THICKFRAME;

		if (createInfo.Maximized)
			style |= WS_MAXIMIZE;

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

		std::wstring title = std::wstring(createInfo.Title.begin(), createInfo.Title.end());

		m_WindowHandle = CreateWindowExW(
			exStyle,
			MAKEINTATOM(windowClass),
			title.c_str(),
			style,
			windowX,
			windowY,
			m_Width,
			m_Height,
			NULL,
			NULL,
			g_Instance,
			NULL);

		if (!m_WindowHandle)
		{
			FLUX_ASSERT(false, "Failed to create window ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			return;
		}

		if (!SetPropW(m_WindowHandle, L"Window", this))
			FLUX_ASSERT(false, "SetPropW failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));

		if (IsWindows7OrGreater())
		{
			if (!ChangeWindowMessageFilterEx(m_WindowHandle, WM_DROPFILES, MSGFLT_ALLOW, NULL))
				FLUX_ASSERT(false, "ChangeWindowMessageFilterEx failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			
			if (!ChangeWindowMessageFilterEx(m_WindowHandle, WM_COPYDATA, MSGFLT_ALLOW, NULL))
				FLUX_ASSERT(false, "ChangeWindowMessageFilterEx failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));

			if (!ChangeWindowMessageFilterEx(m_WindowHandle, WM_COPYGLOBALDATA, MSGFLT_ALLOW, NULL))
				FLUX_ASSERT(false, "ChangeWindowMessageFilterEx failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
		}

		DragAcceptFiles(m_WindowHandle, TRUE);

		RECT clientRect;
		if (GetClientRect(m_WindowHandle, &clientRect))
		{
			m_Width = clientRect.right;
			m_Height = clientRect.bottom;
		}

		ShowWindow(m_WindowHandle, SW_SHOWNA);
	}

	WindowsWindow::~WindowsWindow()
	{
		if (!IsWindow(m_WindowHandle))
		{
			FLUX_WARNING("Invalid window handle!");
			return;
		}

		if (!DestroyWindow(m_WindowHandle))
			FLUX_ASSERT(false, "DestroyWindow failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
	}

	int32 WindowsWindow::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_SIZE:
		{
			const uint32 width = LOWORD(lParam);
			const uint32 height = HIWORD(lParam);

			if (width != m_Width || height != m_Height)
			{
				m_Width = width;
				m_Height = height;

				for (auto& callback : m_SizeCallbacks)
					callback(width, height);
			}
			break;
		}
		case WM_CLOSE:
		{
			for (auto& callback : m_CloseCallbacks)
				callback();
			break;
		}
		}

		return static_cast<int32>(DefWindowProcW(m_WindowHandle, uMsg, wParam, lParam));
	}

}

#endif