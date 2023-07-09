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
		FLUX_ASSERT_IS_THREAD(m_ThreadID);
		FLUX_ASSERT(::IsWindow(m_WindowHandle), "Invalid window handle.");

		RemovePropW(m_WindowHandle, L"Window");

		if (!DestroyWindow(m_WindowHandle))
			FLUX_ASSERT(false, "DestroyWindow failed. ({0})", Platform::GetErrorMessage());
	}

	WindowMenu WindowsWindow::CreateMenu() const
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);

		return static_cast<WindowMenu>(::CreateMenu());
	}

	bool WindowsWindow::SetMenu(WindowMenu menu) const
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);
		FLUX_ASSERT(::IsWindow(m_WindowHandle), "Invalid window handle.");

		return ::SetMenu(m_WindowHandle, static_cast<HMENU>(menu));
	}

	bool WindowsWindow::AddMenu(WindowMenu menu, uint32 itemID, const char* name, bool disabled) const
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);
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
		FLUX_ASSERT_IS_THREAD(m_ThreadID);
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
		FLUX_ASSERT_IS_THREAD(m_ThreadID);
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
		FLUX_ASSERT_IS_THREAD(m_ThreadID);
		FLUX_ASSERT(::IsWindow(m_WindowHandle), "Invalid window handle.");

		::ShowWindow(m_WindowHandle, visible ? SW_SHOWNA : SW_HIDE);
	}

	bool WindowsWindow::IsVisible() const
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);
		FLUX_ASSERT(::IsWindow(m_WindowHandle), "Invalid window handle.");

		return ::IsWindowVisible(m_WindowHandle);
	}

	void WindowsWindow::AddCloseCallback(const WindowCloseCallback& callback)
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);

		m_CloseCallbacks.push_back(callback);
	}

	void WindowsWindow::AddMinimizeCallback(const WindowMinimizeCallback& callback)
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);

		m_MinimizeCallbacks.push_back(callback);
	}

	void WindowsWindow::AddSizeCallback(const WindowSizeCallback& callback)
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);

		m_SizeCallbacks.push_back(callback);
	}

	void WindowsWindow::AddMenuCallback(const WindowMenuCallback& callback)
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);

		m_MenuCallbacks.push_back(callback);
	}

	void WindowsWindow::AddDropCallback(const WindowDropCallback& callback)
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);

		m_DropCallbacks.push_back(callback);
	}

	void WindowsWindow::AddKeyCallback(const KeyCallback& callback)
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);

		m_KeyCallbacks.push_back(callback);
	}

	void WindowsWindow::AddMouseButtonCallback(const MouseButtonCallback& callback)
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);

		m_MouseButtonCallbacks.push_back(callback);
	}

	void WindowsWindow::AddMouseMoveCallback(const MouseMoveCallback& callback)
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);

		m_MouseMoveCallbacks.push_back(callback);
	}

	int32 WindowsWindow::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		FLUX_ASSERT_IS_THREAD(m_ThreadID);

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
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			int32 scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
			if (!scancode)
				scancode = MapVirtualKeyW((UINT)wParam, MAPVK_VK_TO_VSC);

			if (scancode == 0x54)
				scancode = 0x137;
			if (scancode == 0x146)
				scancode = 0x45;
			if (scancode == 0x136)
				scancode = 0x36;

			int32 key = Platform::GetKeyCode(scancode);

			if (wParam == VK_CONTROL)
			{
				if (HIWORD(lParam) & KF_EXTENDED)
				{
					key = FLUX_KEY_RIGHT_CONTROL;
				}
				else
				{
					const DWORD time = GetMessageTime();

					MSG msg;
					while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
					{
						if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN || msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP)
						{
							if (msg.wParam == VK_MENU && (HIWORD(msg.lParam) & KF_EXTENDED) && msg.time == time)
								break;
						}
					}

					key = FLUX_KEY_LEFT_CONTROL;
				}
			}
			else if (wParam == VK_PROCESSKEY)
			{
				break;
			}

			int32 action;
			if (HIWORD(lParam) & KF_UP)
				action = FLUX_ACTION_RELEASE;
			else
				action = FLUX_ACTION_PRESS;

			int32 mods = 0;
			if (GetKeyState(VK_SHIFT) & 0x8000)
				mods |= FLUX_MOD_SHIFT;
			if (GetKeyState(VK_CONTROL) & 0x8000)
				mods |= FLUX_MOD_CONTROL;
			if (GetKeyState(VK_MENU) & 0x8000)
				mods |= FLUX_MOD_ALT;
			if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
				mods |= FLUX_MOD_SUPER;
			if (GetKeyState(VK_CAPITAL) & 1)
				mods |= FLUX_MOD_CAPS_LOCK;
			if (GetKeyState(VK_NUMLOCK) & 1)
				mods |= FLUX_MOD_NUM_LOCK;

			if (action == FLUX_ACTION_RELEASE && wParam == VK_SHIFT)
			{
				for (auto& callback : m_KeyCallbacks)
					callback(FLUX_KEY_LEFT_SHIFT, scancode, action, mods);

				for (auto& callback : m_KeyCallbacks)
					callback(FLUX_KEY_RIGHT_SHIFT, scancode, action, mods);
			}
			else if (wParam == VK_SNAPSHOT)
			{
				for (auto& callback : m_KeyCallbacks)
					callback(key, scancode, FLUX_ACTION_PRESS, mods);

				for (auto& callback : m_KeyCallbacks)
					callback(key, scancode, FLUX_ACTION_RELEASE, mods);
			}
			else
			{
				for (auto& callback : m_KeyCallbacks)
					callback(key, scancode, action, mods);
			}

			break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			int32 button;
			if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
				button = FLUX_MOUSE_BUTTON_LEFT;
			else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
				button = FLUX_MOUSE_BUTTON_RIGHT;
			else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
				button = FLUX_MOUSE_BUTTON_MIDDLE;
			else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
				button = FLUX_MOUSE_BUTTON_4;
			else
				button = FLUX_MOUSE_BUTTON_5;

			int32 action;
			if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN || uMsg == WM_XBUTTONDOWN)
				action = FLUX_ACTION_PRESS;
			else
				action = FLUX_ACTION_RELEASE;

			int32 mods = 0;
			if (GetKeyState(VK_SHIFT) & 0x8000)
				mods |= FLUX_MOD_SHIFT;
			if (GetKeyState(VK_CONTROL) & 0x8000)
				mods |= FLUX_MOD_CONTROL;
			if (GetKeyState(VK_MENU) & 0x8000)
				mods |= FLUX_MOD_ALT;
			if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
				mods |= FLUX_MOD_SUPER;
			if (GetKeyState(VK_CAPITAL) & 1)
				mods |= FLUX_MOD_CAPS_LOCK;
			if (GetKeyState(VK_NUMLOCK) & 1)
				mods |= FLUX_MOD_NUM_LOCK;

			for (auto& callback : m_MouseButtonCallbacks)
				callback(button, action, mods);

			break;
		}
		case WM_MOUSEMOVE:
		{
			const int32 x = GET_X_LPARAM(lParam);
			const int32 y = GET_Y_LPARAM(lParam);

			if (!m_CursorTracked)
			{
				TRACKMOUSEEVENT tme = {};
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = m_WindowHandle;
				TrackMouseEvent(&tme);

				m_CursorTracked = true;
				
				// TODO: callback?
			}

			for (auto& callback : m_MouseMoveCallbacks)
				callback(x, y);

			break;
		}
		case WM_MOUSELEAVE:
		{
			m_CursorTracked = false;
			// TODO: callback?
			break;
		}
		case WM_DROPFILES:
		{
			HDROP drop = (HDROP)wParam;

			const uint32 count = DragQueryFileW(drop, 0xffffffff, NULL, 0);
				
			POINT point;
			DragQueryPoint(drop, &point);

			for (auto& callback : m_MouseMoveCallbacks)
				callback(point.x, point.y);

			char** paths = (char**)calloc(count, sizeof(char*));

			for (uint32 i = 0; i < count; i++)
			{
				const size_t length = static_cast<size_t>(DragQueryFileW(drop, i, NULL, 0));

				wchar_t* buffer = (wchar_t*)calloc(length + 1, sizeof(wchar_t));
				DragQueryFileW(drop, i, buffer, length + 1);

				paths[i] = (char*)calloc(length + 1, 1);
				WideCharToMultiByte(CP_UTF8, 0, buffer, -1, paths[i], length + 1, NULL, NULL);

				free(buffer);
			}

			for (auto& callback : m_DropCallbacks)
				callback((const char**)paths, count);

			for (uint32 i = 0; i < count; i++)
				free(paths[i]);
			free(paths);

			DragFinish(drop);
			return 0;
		}
		}

		return static_cast<int32>(DefWindowProcW(m_WindowHandle, uMsg, wParam, lParam));
	}

}

#endif