#include "FluxPCH.h"

#ifdef FLUX_PLATFORM_WINDOWS

#include "WindowsWindow.h"

#include "Flux/Runtime/Core/Events/KeyEvent.h"
#include "Flux/Runtime/Core/Events/MouseEvent.h"
#include "Flux/Runtime/Core/Events/WindowEvent.h"

#include <WinUser.h>
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

#if 0
		Ref<WindowsWindow> parentWindow = createInfo.ParentWindow.As<WindowsWindow>();
		if (parentWindow)
			m_EventQueue = parentWindow->m_EventQueue;
#endif

		m_ThreadID = Platform::GetCurrentThreadID();

#if 0
		auto monitorHandles = Platform::GetMonitorHandles();
		MonitorHandle secondMonitorHandle = monitorHandles[1];
		MonitorInfo secondMonitorInfo = Platform::GetMonitorInfo(secondMonitorHandle);
		__debugbreak();
#endif

		if (createInfo.Mode == WindowMode::Fullscreen)
		{
			FLUX_VERIFY(false, "Not implemented");
			// TODO
		}

		m_Style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX;

		if (createInfo.Decorated)
		{
			m_Style |= WS_CAPTION;

			if (createInfo.Resizable)
				m_Style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
		}
		else
		{
			m_Style |= WS_POPUP;
		}

		if (createInfo.Maximized)
			m_Style |= WS_MAXIMIZE;

		m_ExStyle |= WS_EX_APPWINDOW;

		int32 windowX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (m_Width / 2);
		int32 windowY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (m_Height / 2);

		RECT rect = { 0, 0, static_cast<LONG>(m_Width), static_cast<LONG>(m_Height) };
		if (AdjustWindowRectEx(&rect, m_Style, FALSE, m_ExStyle))
		{
			windowX = windowX + rect.left;
			windowY = windowY + rect.top;
			m_Width = rect.right - rect.left;
			m_Height = rect.bottom - rect.top;
		}

		m_PositionX = windowX;
		m_PositionY = windowY;

		ATOM windowClass = static_cast<ATOM>(Platform::GetWindowClass());
		if (!windowClass)
		{
			FLUX_ASSERT(false, "Window class is NULL!");
			return;
		}

		wchar_t* title = new wchar_t[createInfo.Title.size() + 1];
		MultiByteToWideChar(CP_UTF8, 0, createInfo.Title.c_str(), -1, title, static_cast<int32>(createInfo.Title.size()) + 1);

		HWND hWndParent = static_cast<HWND>(createInfo.ParentWindow ? createInfo.ParentWindow->GetNativeHandle() : NULL);

		m_WindowHandle = CreateWindowExW(
			m_ExStyle,
			MAKEINTATOM(windowClass),
			title,
			m_Style,
			windowX,
			windowY,
			m_Width,
			m_Height,
			hWndParent,
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

		m_CursorImageMap[CursorShape::Arrow] = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(OCR_NORMAL), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		m_CursorImageMap[CursorShape::IBeam] = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(OCR_IBEAM), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		m_CursorImageMap[CursorShape::Cross] = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(OCR_CROSS), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		m_CursorImageMap[CursorShape::Hand] = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(OCR_HAND), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		m_CursorImageMap[CursorShape::ResizeEW] = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(OCR_SIZEWE), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		m_CursorImageMap[CursorShape::ResizeNS] = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(OCR_SIZENS), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		m_CursorImageMap[CursorShape::ResizeNWSE] = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(OCR_SIZENWSE), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		m_CursorImageMap[CursorShape::ResizeNESW] = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(OCR_SIZENESW), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		m_CursorImageMap[CursorShape::ResizeAll] = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(OCR_SIZEALL), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		m_CursorImageMap[CursorShape::NotAllowed] = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(OCR_NO), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);

		// Set default cursor
		::SetCursor(m_CursorImageMap[m_CursorShape]);
	}

	WindowsWindow::~WindowsWindow()
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		for (auto& [cursorShape, cursorHandle] : m_CursorImageMap)
			DestroyCursor(cursorHandle);
		m_CursorImageMap.clear();

		RemovePropW(m_WindowHandle, L"Window");
		DestroyWindow(m_WindowHandle);
	}

	void WindowsWindow::SetSize(uint32 width, uint32 height)
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		if (m_Width != width || m_Height != height)
		{
			RECT rect = { 0, 0, (LONG)width, (LONG)height };
			::AdjustWindowRectEx(&rect, m_Style, FALSE, m_ExStyle);
			m_Width = rect.right - rect.left;
			m_Height = rect.bottom - rect.top;

			::SetWindowPos(m_WindowHandle, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}
	}

	void WindowsWindow::SetPosition(uint32 x, uint32 y)
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		if (m_PositionX != x || m_PositionY != y)
		{
			RECT rect = { static_cast<LONG>(x), static_cast<LONG>(y), static_cast<LONG>(x), static_cast<LONG>(y) };
			::AdjustWindowRectEx(&rect, m_Style, FALSE, m_ExStyle);
			m_PositionX = rect.left;
			m_PositionY = rect.top;

			::SetWindowPos(m_WindowHandle, NULL, m_PositionX, m_PositionY, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
		}
	}

	std::pair<uint32, uint32> WindowsWindow::GetPosition() const
	{
		return { m_PositionX, m_PositionY };
	}

	void WindowsWindow::SetTitle(const std::string& title)
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		std::lock_guard<std::mutex> lock(m_TitleMutex);

		if (m_Title != title)
		{
			wchar_t* buffer = new wchar_t[title.size() + 1];
			MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, buffer, static_cast<int32>(title.size()) + 1);
			SetWindowTextW(m_WindowHandle, buffer);
			delete[] buffer;

			m_Title = title;
		}
	}

	const std::string& WindowsWindow::GetTitle()
	{
		std::lock_guard<std::mutex> lock(m_TitleMutex);
		return m_Title;
	}

	void WindowsWindow::SetVisible(bool visible) const
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		::ShowWindow(m_WindowHandle, visible ? SW_SHOWNA : SW_HIDE);
	}

	bool WindowsWindow::IsVisible() const
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		return ::IsWindowVisible(m_WindowHandle);
	}

	void WindowsWindow::SetFocus()
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		::BringWindowToTop(m_WindowHandle);
		::SetForegroundWindow(m_WindowHandle);
		::SetFocus(m_WindowHandle);
	}

	bool WindowsWindow::IsFocused() const
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);
		FLUX_VERIFY(false, "Not implemented!");
		return false;
	}

	WindowMenu WindowsWindow::CreateMenu() const
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		return static_cast<WindowMenu>(::CreateMenu());
	}

	bool WindowsWindow::SetMenu(WindowMenu menu) const
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		return ::SetMenu(m_WindowHandle, static_cast<HMENU>(menu));
	}

	bool WindowsWindow::AddMenu(WindowMenu menu, uint32 menuID, const char* name, bool disabled) const
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_STRING | (disabled ? MF_DISABLED : 0), (UINT_PTR)menuID, name))
		{
			FLUX_VERIFY(false, "AppendMenuA failed. ({0})", Platform::GetErrorMessage());
			return false;
		}
		return true;
	}

	bool WindowsWindow::AddMenuSeparator(WindowMenu menu) const
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_SEPARATOR | MF_BYPOSITION, NULL, NULL))
		{
			FLUX_VERIFY(false, "AppendMenuA failed. ({0})", Platform::GetErrorMessage());
			return false;
		}
		return true;
	}

	bool WindowsWindow::AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name, bool disabled) const
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_POPUP | (disabled ? MF_DISABLED : 0), (UINT_PTR)childMenu, name))
		{
			FLUX_VERIFY(false, "AppendMenuA failed. ({0})", Platform::GetErrorMessage());
			return false;
		}
		return true;
	}

	void WindowsWindow::SetCursorShape(CursorShape shape)
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		if (m_CursorShape != shape)
		{
			::SetCursor(m_CursorImageMap[shape]);
			m_CursorShape = shape;
		}
	}

	void WindowsWindow::SetEventQueue(Ref<EventQueue> eventQueue)
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		m_EventQueue = eventQueue;
	}

	int32 WindowsWindow::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		FLUX_CHECK_IS_IN_THREAD(m_ThreadID);

		switch (uMsg)
		{
		case WM_COMMAND:
		{
			SubmitEvent<WindowMenuEvent>(this, m_Menu, (uint32)wParam);
			break;
		}
		case WM_SETFOCUS:
		{
			SubmitEvent<WindowFocusEvent>(this, true);
			return FALSE;
		}
		case WM_KILLFOCUS:
		{
			SubmitEvent<WindowFocusEvent>(this, false);
			return FALSE;
		}
		case WM_SIZE:
		{
			const uint32 width = LOWORD(lParam);
			const uint32 height = HIWORD(lParam);

			if (width != m_Width || height != m_Height)
			{
				m_Width = width;
				m_Height = height;

				bool minimized = wParam == SIZE_MINIMIZED;
				bool maximized = wParam == SIZE_MAXIMIZED || (m_Maximized && wParam != SIZE_RESTORED);

				if (m_Minimized != minimized)
					SubmitEvent<WindowMinimizeEvent>(this, minimized);

				if (m_Maximized != maximized)
					SubmitEvent<WindowMaximizeEvent>(this, maximized);

				SubmitEvent<WindowResizeEvent>(this, width, height);

				m_Minimized = minimized;
				m_Maximized = maximized;
			}

			return FALSE;
		}
		case WM_ERASEBKGND:
		{
			return TRUE;
		}
		case WM_CLOSE:
		{
			SubmitEvent<WindowCloseEvent>(this);
			return FALSE;
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

			if (HIWORD(lParam) & KF_UP)
				SubmitEvent<KeyReleasedEvent>(this, (KeyCode)key);
			else
				SubmitEvent<KeyPressedEvent>(this, (KeyCode)key);
			break;
		}
		case WM_CHAR:
		case WM_SYSCHAR:
		{
			if (IS_HIGH_SURROGATE(wParam))
			{
				m_HighSurrogate = (WCHAR)wParam;
				break;
			}
			else
			{
				char32 codepoint = 0;

				if (IS_LOW_SURROGATE(wParam))
				{
					if (m_HighSurrogate)
					{
						codepoint += (m_HighSurrogate - HIGH_SURROGATE_START) << 10;
						codepoint += (WCHAR)wParam - LOW_SURROGATE_START;
						codepoint += 0x10000;
					}
				}
				else
				{
					codepoint = (WCHAR)wParam;
				}

				m_HighSurrogate = 0;

				if (uMsg != WM_SYSCHAR)
					SubmitEvent<KeyTypedEvent>(this, codepoint);
			}

			return FALSE;
		}
		case WM_UNICHAR:
		{
			if (wParam == UNICODE_NOCHAR)
				return TRUE;

			SubmitEvent<KeyTypedEvent>(this, (char32)wParam);
			return FALSE;
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

			uint32 i;
			for (i = 0; i <= FLUX_MOUSE_BUTTON_LAST; i++)
			{
				if (m_MouseButtons[i])
					break;
			}

			if (i > FLUX_MOUSE_BUTTON_LAST)
				SetCapture(m_WindowHandle);

			bool pressed = uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN || uMsg == WM_XBUTTONDOWN;
			m_MouseButtons[button] = pressed;

			if (pressed)
				SubmitEvent<MouseButtonPressedEvent>(this, (MouseButtonCode)button);
			else
				SubmitEvent<MouseButtonReleasedEvent>(this, (MouseButtonCode)button);

			for (i = 0; i <= FLUX_MOUSE_BUTTON_LAST; i++)
			{
				if (m_MouseButtons[i])
					break;
			}

			if (i > FLUX_MOUSE_BUTTON_LAST)
				ReleaseCapture();

			if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP)
				return TRUE;

			return FALSE;
		}
		case WM_MOUSEMOVE:
		{
			const int32 x = GET_X_LPARAM(lParam);
			const int32 y = GET_Y_LPARAM(lParam);

			SubmitEvent<MouseMovedEvent>(this, (float)x, (float)y);
			return FALSE;
		}
		case WM_MOUSEWHEEL:
		{
			const float x = 0.0f;
			const float y = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;

			SubmitEvent<MouseScrolledEvent>(this, (float)x, (float)y);
			return FALSE;
		}
		case WM_MOUSEHWHEEL:
		{
			const float x = -GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
			const float y = 0.0f;

			SubmitEvent<MouseScrolledEvent>(this, (float)x, (float)y);
			return FALSE;
		}
		case WM_SETCURSOR:
		{
			if (LOWORD(lParam) == HTCLIENT)
			{
				::SetCursor(m_CursorImageMap[m_CursorShape]);
				return TRUE;
			}
			break;
		}
		}

		return static_cast<int32>(DefWindowProcW(m_WindowHandle, uMsg, wParam, lParam));
	}

}

#endif