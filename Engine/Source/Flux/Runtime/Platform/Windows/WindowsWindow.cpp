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

		int32 wTitleSize = MultiByteToWideChar(CP_UTF8, 0, createInfo.Title.c_str(), -1, NULL, 0);
		wchar_t* wTitle = new wchar_t[wTitleSize];
		MultiByteToWideChar(CP_UTF8, 0, createInfo.Title.c_str(), -1, wTitle, wTitleSize);

		HWND hWndParent = static_cast<HWND>(createInfo.ParentWindow ? createInfo.ParentWindow->GetNativeHandle() : NULL);

		m_WindowHandle = CreateWindowExW(
			exStyle,
			MAKEINTATOM(windowClass),
			wTitle,
			style,
			windowX,
			windowY,
			m_Width,
			m_Height,
			hWndParent,
			NULL,
			g_Instance,
			NULL);

		delete[] wTitle;

		if (!m_WindowHandle)
		{
			FLUX_ASSERT(false, "Failed to create window. ({0})", Platform::GetErrorMessage());
			return;
		}

		if (!SetPropW(m_WindowHandle, L"Window", this))
			FLUX_ASSERT(false, "SetPropW failed. ({0})", Platform::GetErrorMessage());

		if (IsWindows7OrGreater())
		{
			if (!ChangeWindowMessageFilterEx(m_WindowHandle, WM_DROPFILES, MSGFLT_ALLOW, NULL))
				FLUX_ASSERT(false, "ChangeWindowMessageFilterEx failed. ({0})", Platform::GetErrorMessage());
			
			if (!ChangeWindowMessageFilterEx(m_WindowHandle, WM_COPYDATA, MSGFLT_ALLOW, NULL))
				FLUX_ASSERT(false, "ChangeWindowMessageFilterEx failed. ({0})", Platform::GetErrorMessage());

			if (!ChangeWindowMessageFilterEx(m_WindowHandle, WM_COPYGLOBALDATA, MSGFLT_ALLOW, NULL))
				FLUX_ASSERT(false, "ChangeWindowMessageFilterEx failed. ({0})", Platform::GetErrorMessage());
		}

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
		FLUX_ASSERT_ON_THREAD(m_ThreadID);
		FLUX_ASSERT(::IsWindow(m_WindowHandle), "Invalid window handle.");

		RemovePropW(m_WindowHandle, L"Window");

		if (!DestroyWindow(m_WindowHandle))
			FLUX_ASSERT(false, "DestroyWindow failed. ({0})", Platform::GetErrorMessage());
	}

	WindowMenu WindowsWindow::CreateMenu() const
	{
		FLUX_ASSERT_ON_THREAD(m_ThreadID);

		return static_cast<WindowMenu>(::CreateMenu());
	}

	bool WindowsWindow::SetMenu(WindowMenu menu) const
	{
		FLUX_ASSERT_ON_THREAD(m_ThreadID);
		FLUX_ASSERT(::IsWindow(m_WindowHandle), "Invalid window handle.");

		return ::SetMenu(m_WindowHandle, static_cast<HMENU>(menu));
	}

	bool WindowsWindow::AddMenu(WindowMenu menu, uint32 itemID, const char* name, bool disabled) const
	{
		FLUX_ASSERT_ON_THREAD(m_ThreadID);
		FLUX_ASSERT(::IsWindow(m_WindowHandle), "Invalid window handle.");

		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_STRING | (disabled ? MF_DISABLED : 0), (UINT_PTR)itemID, name))
		{
			FLUX_ASSERT(false, "AppendMenuA failed. ({0})", Platform::GetErrorMessage());
			return false;
		}
		return true;
	}

	bool WindowsWindow::AddMenuSeparator(WindowMenu menu) const
	{
		FLUX_ASSERT_ON_THREAD(m_ThreadID);
		FLUX_ASSERT(::IsWindow(m_WindowHandle), "Invalid window handle.");

		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_SEPARATOR | MF_BYPOSITION, NULL, NULL))
		{
			FLUX_ASSERT(false, "AppendMenuA failed. ({0})", Platform::GetErrorMessage());
			return false;
		}
		return true;
	}

	bool WindowsWindow::AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name, bool disabled) const
	{
		FLUX_ASSERT_ON_THREAD(m_ThreadID);
		FLUX_ASSERT(::IsWindow(m_WindowHandle), "Invalid window handle.");

		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_POPUP | (disabled ? MF_DISABLED : 0), (UINT_PTR)childMenu, name))
		{
			FLUX_ASSERT(false, "AppendMenuA failed. ({0})", Platform::GetErrorMessage());
			return false;
		}
		return true;
	}

	void WindowsWindow::SetVisible(bool visible) const
	{
		FLUX_ASSERT_ON_THREAD(m_ThreadID);
		FLUX_ASSERT(::IsWindow(m_WindowHandle), "Invalid window handle.");

		::ShowWindow(m_WindowHandle, visible ? SW_SHOWNA : SW_HIDE);
	}

	bool WindowsWindow::IsVisible() const
	{
		FLUX_ASSERT_ON_THREAD(m_ThreadID);
		FLUX_ASSERT(::IsWindow(m_WindowHandle), "Invalid window handle.");

		return ::IsWindowVisible(m_WindowHandle);
	}

	void WindowsWindow::AddCloseCallback(const WindowCloseCallback& callback)
	{
		FLUX_ASSERT_ON_THREAD(m_ThreadID);

		m_CloseCallbacks.push_back(callback);
	}

	void WindowsWindow::AddMinimizeCallback(const WindowMinimizeCallback& callback)
	{
		FLUX_ASSERT_ON_THREAD(m_ThreadID);

		m_MinimizeCallbacks.push_back(callback);
	}

	void WindowsWindow::AddSizeCallback(const WindowSizeCallback& callback)
	{
		FLUX_ASSERT_ON_THREAD(m_ThreadID);

		m_SizeCallbacks.push_back(callback);
	}

	void WindowsWindow::AddMenuCallback(const WindowMenuCallback& callback)
	{
		FLUX_ASSERT_ON_THREAD(m_ThreadID);

		m_MenuCallbacks.push_back(callback);
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

			const bool minimized = wParam == SIZE_MINIMIZED;

			if (m_Minimized != minimized)
			{
				m_Minimized = minimized;
				
				for (auto& callback : m_MinimizeCallbacks)
					callback(minimized);
			}

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
			return 0;
		}
		}

		return static_cast<int32>(DefWindowProcW(m_WindowHandle, uMsg, wParam, lParam));
	}

}

#endif