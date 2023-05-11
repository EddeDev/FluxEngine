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

		int32 wTitleSize = MultiByteToWideChar(CP_UTF8, 0, createInfo.Title.c_str(), -1, NULL, 0);
		wchar_t* wTitle = new wchar_t[wTitleSize];
		MultiByteToWideChar(CP_UTF8, 0, createInfo.Title.c_str(), -1, wTitle, wTitleSize);

		m_WindowHandle = CreateWindowExW(
			exStyle,
			MAKEINTATOM(windowClass),
			wTitle,
			style,
			windowX,
			windowY,
			m_Width,
			m_Height,
			NULL,
			NULL,
			g_Instance,
			NULL);

		delete[] wTitle;

		if (!m_WindowHandle)
		{
			FLUX_ASSERT(false, "Failed to create window. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			return;
		}

		if (!SetPropW(m_WindowHandle, L"Window", this))
			FLUX_ASSERT(false, "SetPropW failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));

		if (IsWindows7OrGreater())
		{
			if (!ChangeWindowMessageFilterEx(m_WindowHandle, WM_DROPFILES, MSGFLT_ALLOW, NULL))
				FLUX_ASSERT(false, "ChangeWindowMessageFilterEx failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			
			if (!ChangeWindowMessageFilterEx(m_WindowHandle, WM_COPYDATA, MSGFLT_ALLOW, NULL))
				FLUX_ASSERT(false, "ChangeWindowMessageFilterEx failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));

			if (!ChangeWindowMessageFilterEx(m_WindowHandle, WM_COPYGLOBALDATA, MSGFLT_ALLOW, NULL))
				FLUX_ASSERT(false, "ChangeWindowMessageFilterEx failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
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
			FLUX_ASSERT(false, "DestroyWindow failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
	}

	WindowMenu WindowsWindow::CreateMenu() const
	{
		return static_cast<WindowMenu>(::CreateMenu());
	}

	bool WindowsWindow::SetMenu(WindowMenu menu) const
	{
		return ::SetMenu(m_WindowHandle, static_cast<HMENU>(menu));
	}

	bool WindowsWindow::AddMenu(WindowMenu menu, uint32 itemID, const char* name) const
	{
		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_STRING, (UINT_PTR)itemID, name))
		{
			FLUX_ASSERT(false, "AppendMenuA failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			return false;
		}
		return true;
	}

	bool WindowsWindow::AddMenuSeparator(WindowMenu menu) const
	{
		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_SEPARATOR | MF_BYPOSITION, NULL, NULL))
		{
			FLUX_ASSERT(false, "AppendMenuA failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			return false;
		}
		return true;
	}

	bool WindowsWindow::AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name) const
	{
		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_POPUP, (UINT_PTR)childMenu, name))
		{
			FLUX_ASSERT(false, "AppendMenuA failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			return false;
		}
		return true;
	}

	int32 WindowsWindow::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_COMMAND:
		{
			for (auto& callback : m_MenuCallbacks)
				callback(m_Menu, (uint32)wParam);
			break;
		}
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