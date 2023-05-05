#include "FluxPCH.h"
#include "WindowsWindow.h"

#include "Flux/Core/Platform.h"

#ifdef FLUX_PLATFORM_WINDOWS

#include <VersionHelpers.h>

#ifndef WM_COPYGLOBALDATA
	#define WM_COPYGLOBALDATA 0x0049
#endif

namespace Flux {

	static const wchar_t* s_WindowClassName = L"FluxWindow";

	extern HINSTANCE g_Instance;

	WindowsWindow::WindowsWindow(const WindowCreateInfo& createInfo)
	{
		if (s_WindowCount == 0)
		{
			WNDCLASSEXW windowClass = {};
			windowClass.cbSize = sizeof(WNDCLASSEXW);
			windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			windowClass.lpfnWndProc = WindowProc;
			windowClass.hInstance = g_Instance;
			windowClass.hIcon = (HICON)LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
			windowClass.hCursor = LoadCursorW(g_Instance, IDC_ARROW);
			windowClass.lpszClassName = s_WindowClassName;

			s_WindowClass = RegisterClassExW(&windowClass);
		}

		m_Data.Width = createInfo.Width;
		m_Data.Height = createInfo.Height;
		m_Data.Title = createInfo.Title;

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

		int32 windowX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (m_Data.Width / 2);
		int32 windowY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (m_Data.Height / 2);

		RECT rect = { 0, 0, static_cast<LONG>(m_Data.Width), static_cast<LONG>(m_Data.Height) };
		if (AdjustWindowRectEx(&rect, style, FALSE, exStyle))
		{
			windowX = windowX + rect.left;
			windowY = windowY + rect.top;
			m_Data.Width = rect.right - rect.left;
			m_Data.Height = rect.bottom - rect.top;
		}

		std::wstring title = std::wstring(createInfo.Title.begin(), createInfo.Title.end());

		m_WindowHandle = CreateWindowExW(
			exStyle,
			MAKEINTATOM(s_WindowClass),
			title.c_str(),
			style,
			windowX,
			windowY,
			m_Data.Width,
			m_Data.Height,
			NULL,
			NULL,
			g_Instance,
			NULL);

		if (!m_WindowHandle)
		{
			FLUX_ERROR("Failed to create window ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			return;
		}

		s_WindowCount++;

		SetPropW(m_WindowHandle, L"Data", &m_Data);

		if (IsWindows7OrGreater())
		{
			ChangeWindowMessageFilterEx(m_WindowHandle, WM_DROPFILES, MSGFLT_ALLOW, NULL);
			ChangeWindowMessageFilterEx(m_WindowHandle, WM_COPYDATA, MSGFLT_ALLOW, NULL);
			ChangeWindowMessageFilterEx(m_WindowHandle, WM_COPYGLOBALDATA, MSGFLT_ALLOW, NULL);
		}

		DragAcceptFiles(m_WindowHandle, TRUE);

		RECT clientRect;
		if (GetClientRect(m_WindowHandle, &clientRect))
		{
			m_Data.Width = clientRect.right;
			m_Data.Height = clientRect.bottom;
		}

		ShowWindow(m_WindowHandle, SW_SHOWNA);
	}

	WindowsWindow::~WindowsWindow()
	{
		DestroyWindow(m_WindowHandle);

		s_WindowCount--;

		if (s_WindowCount == 0)
			UnregisterClassW(MAKEINTATOM(s_WindowClass), g_Instance);
	}

	void WindowsWindow::ProcessEvents() const
	{
		MSG msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	static void CreateChildMenus(Shared<WindowMenu> menu, HMENU hMenu)
	{
		auto& childMenus = menu->GetChildren();
		for (auto& childMenu : childMenus)
		{
			if (!childMenu)
			{
				FLUX_ASSERT(false, "WindowMenu is nullptr!");
				continue;
			}

			HMENU hChildMenu = CreateMenu();
			CreateChildMenus(childMenu, hChildMenu);

			BOOL result;
			if (childMenu->IsSeparator())
				result = AppendMenuA(hMenu, MF_SEPARATOR | MF_BYPOSITION, (UINT_PTR)childMenu->GetID(), NULL);
			else if (childMenu->GetChildren().empty())
				result = AppendMenuA(hMenu, MF_STRING, (UINT_PTR)childMenu->GetID(), childMenu->GetName().c_str());
			else
				result = AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hChildMenu, childMenu->GetName().c_str());

			if (!result)
			{
				DWORD lastError = GetLastError();
				FLUX_ASSERT(false, "AppendMenuA failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			}
		}
	}

	static Shared<WindowMenu> FindMenuByID(Shared<WindowMenu> rootMenu, uint32 id)
	{
		auto& childMenus = rootMenu->GetChildren();
		for (auto& childMenu : childMenus)
		{
			if (!childMenu)
			{
				FLUX_ASSERT(false, "WindowMenu is nullptr!");
				continue;
			}
			
			if (id == childMenu->GetID())
				return childMenu;

			Shared<WindowMenu> menu = FindMenuByID(childMenu, id);
			if (menu)
				return menu;
		}
		return nullptr;
	}

	void WindowsWindow::SetMenu(Shared<WindowMenu> menu)
	{
		if (!menu)
		{
			FLUX_ASSERT(false, "WindowMenu is nullptr!");
			return;
		}

		if (menu->HasParent() || menu->GetChildren().empty())
			return;

		HMENU hMenu = CreateMenu();
		CreateChildMenus(menu, hMenu);
		::SetMenu(m_WindowHandle, hMenu);
		m_Data.Menu = menu;
	}

	LRESULT CALLBACK WindowsWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowData* data = (WindowData*)GetPropW(hWnd, L"Data");
		if (!data)
			return DefWindowProcW(hWnd, uMsg, wParam, lParam);

		switch (uMsg)
		{
		case WM_SIZE:
		{
			const uint32 width = LOWORD(lParam);
			const uint32 height = HIWORD(lParam);

			if (width != data->Width || height != data->Height)
			{
				data->Width = width;
				data->Height = height;

				for (auto& callback : data->SizeCallbacks)
					callback(width, height);
			}

			break;
		}
		case WM_COMMAND:
		{
			if (data->Menu)
			{
				Shared<WindowMenu> menu = FindMenuByID(data->Menu, (uint32)wParam);
				if (menu)
				{
					auto& callback = menu->GetCallback();
					if (callback)
						callback();
				}
			}
			break;
		}
		case WM_CLOSE:
		{
			for (auto& callback : data->CloseCallbacks)
				callback();
			break;
		}
		}

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

}

#endif