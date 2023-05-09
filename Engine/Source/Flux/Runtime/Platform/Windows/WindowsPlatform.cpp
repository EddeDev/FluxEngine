#include "FluxPCH.h"
#include "WindowsPlatform.h"

#ifdef FLUX_PLATFORM_WINDOWS

#include "WindowsWindow.h"

namespace Flux {

	extern HINSTANCE g_Instance;

	struct WindowsPlatformData
	{
		uint64 TimerOffset;
		uint64 TimerFrequency;

		ATOM WindowClass;
	};

	static WindowsPlatformData s_Data;
	
	void WindowsPlatform::Init()
	{
		if (!QueryPerformanceCounter((LARGE_INTEGER*)&s_Data.TimerOffset))
			FLUX_ASSERT(false, "QueryPerformanceCounter failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
		
		if (!QueryPerformanceFrequency((LARGE_INTEGER*)&s_Data.TimerFrequency))
			FLUX_ASSERT(false, "QueryPerformanceFrequency failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));

		DisableProcessWindowsGhosting();

		WNDCLASSEXW windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEXW);
		windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = g_Instance;
		windowClass.hIcon = (HICON)LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		windowClass.hCursor = LoadCursorW(g_Instance, IDC_ARROW);
		windowClass.lpszClassName = L"FluxWindow";

		s_Data.WindowClass = RegisterClassExW(&windowClass);
		if (!s_Data.WindowClass)
			FLUX_ASSERT(false, "RegisterClassExW failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));

		if (!SUCCEEDED(OleInitialize(NULL)))
			FLUX_ASSERT(false, "Failed to initialize COM library.\n{0}", Platform::GetErrorMessage(Platform::GetLastError()));
	}

	void WindowsPlatform::Shutdown()
	{
		if (!UnregisterClassW(MAKEINTATOM(s_Data.WindowClass), g_Instance))
			FLUX_ASSERT(false, "UnregisterClassExW failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));

		OleUninitialize();
	}

	void WindowsPlatform::PumpMessages()
	{
		MSG msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	LRESULT CALLBACK WindowsPlatform::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowsWindow* window = (WindowsWindow*)GetPropW(hWnd, L"Window");
		if (window)
		{
			if (uMsg == WM_COMMAND)
			{
				auto it = s_Menus.find(window);
				if (it != s_Menus.end())
				{
					auto& data = it->second;
					if (data.Callback)
						data.Callback(data.Menu, (uint32)wParam);
				}
			}

			return window->ProcessMessage(uMsg, wParam, lParam);
		}

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	void WindowsPlatform::Sleep(float seconds)
	{
		DWORD milliseconds = static_cast<DWORD>(seconds * 1000.0f);
		if (milliseconds == 0)
			::SwitchToThread();
		else
			::Sleep(milliseconds);
	}
	
	float WindowsPlatform::GetTime()
	{
		uint64 value = 0;
		FLUX_ASSERT(QueryPerformanceCounter((LARGE_INTEGER*)&value));
		return static_cast<float>(value - s_Data.TimerOffset) / static_cast<float>(s_Data.TimerFrequency);
	}

	uint64 WindowsPlatform::GetNanoTime()
	{
		uint64 value = 0;
		FLUX_ASSERT(QueryPerformanceCounter((LARGE_INTEGER*)&value));
		constexpr uint64 nsPerSecond = 1000 * 1000 * 1000;
		return value * (nsPerSecond / s_Data.TimerFrequency);
	}

	WindowMenu WindowsPlatform::CreateMenu()
	{
		return static_cast<WindowMenu>(::CreateMenu());
	}

	bool WindowsPlatform::SetMenu(Window* window, WindowMenu menu, WindowMenuCallback callback)
	{
		HWND hWnd = static_cast<HWND>(window ? window->GetNativeHandle() : NULL);
		if (!hWnd)
			return false;

		auto& data = s_Menus[window];
		data.Menu = menu;
		data.Callback = callback;

		return ::SetMenu(hWnd, static_cast<HMENU>(menu));
	}

	bool WindowsPlatform::AddMenu(WindowMenu menu, uint32 id, const char* name)
	{
		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_STRING, (UINT_PTR)id, name))
		{
			FLUX_ASSERT(false, "AppendMenuA failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			return false;
		}
		return true;
	}

	bool WindowsPlatform::AddMenuSeparator(WindowMenu menu)
	{
		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_SEPARATOR | MF_BYPOSITION, NULL, NULL))
		{
			FLUX_ASSERT(false, "AppendMenuA failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			return false;
		}
		return true;
	}

	bool WindowsPlatform::AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name)
	{
		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_POPUP, (UINT_PTR)childMenu, name))
		{
			FLUX_ASSERT(false, "AppendMenuA failed ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			return false;
		}
		return true;
	}

	MessageBoxResult WindowsPlatform::MessageBox(MessageBoxButtons buttons, MessageBoxIcon icon, const char* text, const char* caption, Window* window)
	{
		uint32 flags = 0;

		switch (buttons)
		{
		case MessageBoxButtons::AbortRetryIgnore: flags |= MB_ABORTRETRYIGNORE; break;
		case MessageBoxButtons::Ok:               flags |= MB_OK;               break;
		case MessageBoxButtons::OkCancel:         flags |= MB_OKCANCEL;         break;
		case MessageBoxButtons::RetryCancel:      flags |= MB_RETRYCANCEL;      break;
		case MessageBoxButtons::YesNo:            flags |= MB_YESNO;            break;
		case MessageBoxButtons::YesNoCancel:      flags |= MB_YESNOCANCEL;      break;
		}

		switch (icon)
		{
		case MessageBoxIcon::Asterisk:     flags |= MB_ICONASTERISK;    break;
		case MessageBoxIcon::Error:        flags |= MB_ICONERROR;       break;
		case MessageBoxIcon::Exclamation:  flags |= MB_ICONEXCLAMATION; break;
		case MessageBoxIcon::Hand:         flags |= MB_ICONHAND;        break;
		case MessageBoxIcon::Information:  flags |= MB_ICONINFORMATION; break;
		case MessageBoxIcon::Question:     flags |= MB_ICONQUESTION;    break;
		case MessageBoxIcon::Stop:         flags |= MB_ICONSTOP;        break;
		case MessageBoxIcon::Warning:      flags |= MB_ICONWARNING;     break;
		}

		HWND hWnd = static_cast<HWND>(window ? window->GetNativeHandle() : NULL);

		int32 result = ::MessageBoxA(hWnd, text, caption, flags);
		switch (result)
		{
		case IDABORT:    return MessageBoxResult::Abort;
		case IDCANCEL:   return MessageBoxResult::Cancel;
		case IDCONTINUE: return MessageBoxResult::Ok;
		case IDIGNORE:   return MessageBoxResult::Ignore;
		case IDNO:       return MessageBoxResult::No;
		case IDOK:       return MessageBoxResult::Ok;
		case IDRETRY:    return MessageBoxResult::Retry;
		case IDYES:      return MessageBoxResult::Yes;
		}

		return MessageBoxResult::None;
	}

	bool WindowsPlatform::IsDebuggerPresent()
	{
		return ::IsDebuggerPresent();
	}

	void WindowsPlatform::DebugBreak()
	{
		::DebugBreak();
	}

	std::string WindowsPlatform::GetErrorMessage(int32 error)
	{
		if (error == 0)
			return {};

		LPSTR messageBuffer = nullptr;
		DWORD size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
		std::string message(messageBuffer, size);
		LocalFree(messageBuffer);

		if (message[message.size() - 1] == '\n')
			message.pop_back();
		if (message[message.size() - 1] == '\r')
			message.pop_back();
		if (message[message.size() - 1] == '.')
			message.pop_back();

		return message;
	}

	uint32 WindowsPlatform::GetLastError()
	{
		return static_cast<uint32>(::GetLastError());
	}

	std::string WindowsPlatform::GetEnvironmentVariable(const char* variableName)
	{
		static char buffer[65535];
		DWORD bufferSize = ::GetEnvironmentVariableA(variableName, buffer, sizeof(buffer));
		return { buffer, bufferSize };
	}

	bool WindowsPlatform::SetEnvironmentVariable(const char* variableName, const char* value)
	{
		uint32 error = ::SetEnvironmentVariableA(variableName, value);
		if (error == 0)
		{
			FLUX_WARNING("Failed to set environment variable '{0}' to '{1}'", variableName, value);
			return false;
		}
		return true;
	}

	WindowClassHandle WindowsPlatform::GetWindowClass()
	{
		return static_cast<WindowClassHandle>(s_Data.WindowClass);
	}

}

#endif