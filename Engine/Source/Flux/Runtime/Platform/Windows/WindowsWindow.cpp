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

		SetPropW(m_WindowHandle, L"Window", this);

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
			m_Width = clientRect.right;
			m_Height = clientRect.bottom;
		}

		ShowWindow(m_WindowHandle, SW_SHOWNA);
	}

	WindowsWindow::~WindowsWindow()
	{
		DestroyWindow(m_WindowHandle);
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
		m_Menu = menu;
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
		case WM_COMMAND:
		{
			if (m_Menu)
			{
				Shared<WindowMenu> menu = FindMenuByID(m_Menu, (uint32)wParam);
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
			for (auto& callback : m_CloseCallbacks)
				callback();
			break;
		}
		}

		return static_cast<int32>(DefWindowProcW(m_WindowHandle, uMsg, wParam, lParam));
	}

}

#endif