#include "FluxPCH.h"

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Core/Platform.h"

#include "WindowsWindow.h"

namespace Flux {

	extern HINSTANCE g_Instance;

	struct WindowsPlatformData
	{
		uint64 TimerOffset;
		uint64 TimerFrequency;

		ATOM WindowClass;

		ATOM HelperWindowClass;
		HWND HelperWindowHandle;
	};

	static WindowsPlatformData* s_Data = nullptr;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowsWindow* window = (WindowsWindow*)GetPropW(hWnd, L"Window");
		if (window)
			return window->ProcessMessage(uMsg, wParam, lParam);

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	static LRESULT CALLBACK HelperWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	static bool CreateHelperWindow()
	{
		WNDCLASSEXW wc = {};
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = (WNDPROC)HelperWindowProc;
		wc.hInstance = g_Instance;
		wc.lpszClassName = L"HelperWindow";

		s_Data->HelperWindowClass = RegisterClassExW(&wc);
		if (!s_Data->HelperWindowClass)
		{
			FLUX_ASSERT(false, "Failed to register helper window class.");
			return false;
		}

		s_Data->HelperWindowHandle = CreateWindowExW(
			WS_EX_OVERLAPPEDWINDOW,
			MAKEINTATOM(s_Data->HelperWindowClass),
			L"HelperWindow",
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0,
			0,
			1,
			1,
			NULL,
			NULL,
			g_Instance,
			NULL
		);

		if (!s_Data->HelperWindowHandle)
		{
			FLUX_ASSERT(false, "Failed to create helper window.");
			return false;
		}

		ShowWindow(s_Data->HelperWindowHandle, SW_HIDE);

		MSG msg;
		while (PeekMessageW(&msg, s_Data->HelperWindowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		return true;
	}

	void Platform::Init()
	{
		s_Data = new WindowsPlatformData();

		if (!QueryPerformanceCounter((LARGE_INTEGER*)&s_Data->TimerOffset))
			FLUX_ASSERT(false, "QueryPerformanceCounter failed. ({0})", Platform::GetErrorMessage());

		if (!QueryPerformanceFrequency((LARGE_INTEGER*)&s_Data->TimerFrequency))
			FLUX_ASSERT(false, "QueryPerformanceFrequency failed. ({0})", Platform::GetErrorMessage());

		DisableProcessWindowsGhosting();

		WNDCLASSEXW windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEXW);
		windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = g_Instance;
		windowClass.hIcon = (HICON)LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		windowClass.hCursor = LoadCursorW(g_Instance, IDC_ARROW);
		windowClass.lpszClassName = L"FluxWindow";

		s_Data->WindowClass = RegisterClassExW(&windowClass);
		FLUX_ASSERT(s_Data->WindowClass, "RegisterClassExW failed. ({0})", Platform::GetErrorMessage());

		if (!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
			FLUX_ASSERT(false, "Failed to initialize COM library.\n{0}", Platform::GetErrorMessage());

		CreateHelperWindow();
	}

	void Platform::Shutdown()
	{
		DestroyWindow(s_Data->HelperWindowHandle);

		UnregisterClassW(MAKEINTATOM(s_Data->HelperWindowClass), g_Instance);
		UnregisterClassW(MAKEINTATOM(s_Data->WindowClass), g_Instance);

		CoUninitialize();

		delete s_Data;
		s_Data = nullptr;
	}

	bool Platform::WaitMessage()
	{
		return ::WaitMessage();
	}

	bool Platform::PostEmptyEvent()
	{
		return ::PostMessageW(s_Data->HelperWindowHandle, WM_NULL, 0, 0);
	}

	void Platform::PumpMessages()
	{
		MSG msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	void Platform::Sleep(float seconds)
	{
		DWORD milliseconds = static_cast<DWORD>(seconds * 1000.0f);
		if (milliseconds == 0)
			::SwitchToThread();
		else
			::Sleep(milliseconds);
	}

	float Platform::GetTime()
	{
		uint64 value = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&value);
		return static_cast<float>(value - s_Data->TimerOffset) / static_cast<float>(s_Data->TimerFrequency);
	}

	uint64 Platform::GetNanoTime()
	{
		uint64 value = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&value);
		constexpr uint64 nsPerSecond = 1000 * 1000 * 1000;
		return value * (nsPerSecond / s_Data->TimerFrequency);
	}

	DialogResult Platform::MessageBox(MessageBoxButtons buttons, MessageBoxIcon icon, const std::string& text, const std::string& caption)
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

		int32 result = ::MessageBoxA(NULL, text.c_str(), caption.c_str(), flags);
		switch (result)
		{
		case IDABORT:    return DialogResult::Abort;
		case IDCANCEL:   return DialogResult::Cancel;
		case IDCONTINUE: return DialogResult::Ok;
		case IDIGNORE:   return DialogResult::Ignore;
		case IDNO:       return DialogResult::No;
		case IDOK:       return DialogResult::Ok;
		case IDRETRY:    return DialogResult::Retry;
		case IDYES:      return DialogResult::Yes;
		}

		return DialogResult::None;
	}

	bool Platform::IsDebuggerPresent()
	{
		return ::IsDebuggerPresent();
	}

	void Platform::DebugBreak()
	{
		::DebugBreak();
	}

	bool Platform::SetConsoleTitle(const std::string& title)
	{
		return ::SetConsoleTitleA(title.c_str());
	}

	bool Platform::SetThreadName(ThreadHandle handle, std::string_view name)
	{
		wchar_t* buffer = new wchar_t[name.size() + 1];
		MultiByteToWideChar(CP_UTF8, 0, name.data(), -1, buffer, name.size() + 1);
		bool success = SUCCEEDED(::SetThreadDescription((HANDLE)handle, buffer));
		FLUX_VERIFY(success, "SetThreadDescription failed. ({0})", Platform::GetErrorMessage());
		delete[] buffer;
		return true;
	}

	std::string Platform::GetThreadName(ThreadHandle handle)
	{
		wchar_t* name;
		bool success = SUCCEEDED(GetThreadDescription((HANDLE)handle, &name));
		FLUX_VERIFY(success, "GetThreadDescription failed. ({0})", Platform::GetErrorMessage());
		int32 size = WideCharToMultiByte(CP_UTF8, 0, name, -1, NULL, 0, NULL, NULL);
		std::string result;
		result.resize(static_cast<size_t>(size - 1));
		WideCharToMultiByte(CP_UTF8, 0, name, -1, result.data(), size - 1, NULL, NULL);
		return result;
	}

	void Platform::SetThreadPriority(ThreadHandle handle, ThreadPriority priority)
	{
		int32 nPriority = 0;
		switch (priority)
		{
		case ThreadPriority::Lowest:      nPriority = THREAD_PRIORITY_LOWEST; break;
		case ThreadPriority::BelowNormal: nPriority = THREAD_PRIORITY_BELOW_NORMAL; break;
		case ThreadPriority::Normal:      nPriority = THREAD_PRIORITY_NORMAL; break;
		case ThreadPriority::AboveNormal: nPriority = THREAD_PRIORITY_ABOVE_NORMAL; break;
		case ThreadPriority::Highest:     nPriority = THREAD_PRIORITY_HIGHEST; break;
		}
		::SetThreadPriority((HANDLE)handle, nPriority);
	}

	ThreadPriority Platform::GetThreadPriority(ThreadHandle handle)
	{
		int32 nPriority = ::GetThreadPriority((HANDLE)handle);
		switch (nPriority)
		{
		case THREAD_PRIORITY_LOWEST:       return ThreadPriority::Lowest;
		case THREAD_PRIORITY_BELOW_NORMAL: return ThreadPriority::BelowNormal;
		case THREAD_PRIORITY_NORMAL:       return ThreadPriority::Normal;
		case THREAD_PRIORITY_ABOVE_NORMAL: return ThreadPriority::AboveNormal;
		case THREAD_PRIORITY_HIGHEST:      return ThreadPriority::Highest;
		}
		FLUX_ASSERT(false, "Unknown thread priority");
		return ThreadPriority::None;
	}

	ThreadHandle Platform::GetCurrentThread()
	{
		return ::GetCurrentThread();
	}

	ThreadHandle Platform::GetThreadFromID(ThreadID threadID)
	{
		return ::OpenThread(THREAD_QUERY_INFORMATION, FALSE, static_cast<DWORD>(threadID));
	}

	ThreadID Platform::GetThreadID(ThreadHandle handle)
	{
		return static_cast<ThreadID>(::GetThreadId((HANDLE)handle));
	}

	ThreadID Platform::GetCurrentThreadID()
	{
		return static_cast<ThreadID>(::GetCurrentThreadId());
	}

	std::string Platform::GetErrorMessage(uint32 error)
	{
		if (error == 0)
			error = GetLastError();

		LPSTR messageBuffer = nullptr;
		DWORD size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
		std::string message(messageBuffer, size);
		LocalFree(messageBuffer);

		if (message.back() == '\n')
			message.pop_back();
		if (message.back() == '\r')
			message.pop_back();
		if (message.back() == '.')
			message.pop_back();

		return message;
	}

	uint32 Platform::GetLastError()
	{
		return static_cast<uint32>(::GetLastError());
	}

	WindowClassHandle Platform::GetWindowClass()
	{
		return static_cast<WindowClassHandle>(s_Data->WindowClass);
	}

	WindowHandle Platform::GetHelperWindowHandle()
	{
		return s_Data->HelperWindowHandle;
	}

}

#endif