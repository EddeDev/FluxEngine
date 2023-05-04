#include "FluxPCH.h"
#include "WindowsWindow.h"

#ifdef FLUX_PLATFORM_WINDOWS

#include <VersionHelpers.h>

#ifndef WM_COPYGLOBALDATA
	#define WM_COPYGLOBALDATA 0x0049
#endif

namespace Flux {

	namespace Utils {

		std::string GetErrorAsString(DWORD errorMessageID)
		{
			if (errorMessageID == 0)
				return std::string();

			LPSTR messageBuffer = nullptr;
			size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
			std::string message(messageBuffer, size);
			LocalFree(messageBuffer);

			if (message[message.size() - 1] == '\n')
				message.pop_back();
			if (message[message.size() - 1] == '\r')
				message.pop_back();
			return message;
		}

	}

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
			DWORD lastError = GetLastError();
			FLUX_ERROR("Failed to create window ({0}: {1})", lastError, Utils::GetErrorAsString(lastError));
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

	LRESULT CALLBACK WindowsWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowData* data = (WindowData*)GetPropW(hWnd, L"Data");
		if (!data)
			return DefWindowProcW(hWnd, uMsg, wParam, lParam);

		switch (uMsg)
		{
		case WM_CREATE:
		{
			__debugbreak();
			break;
		}
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