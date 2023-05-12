#include "FluxPCH.h"

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Runtime/Core/Platform.h"

#include "WindowsWindow.h"

#include <ShObjIdl.h>

namespace Flux {

	extern HINSTANCE g_Instance;

	static uint64 s_TimerOffset;
	static uint64 s_TimerFrequency;

	static ATOM s_WindowClass;

	static ATOM s_HelperWindowClass;
	static HWND s_HelperWindow;

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

		s_HelperWindowClass = RegisterClassExW(&wc);
		if (!s_HelperWindowClass)
		{
			FLUX_ASSERT(false, "Failed to register helper window class.");
			return false;
		}

		s_HelperWindow = CreateWindowExW(
			WS_EX_OVERLAPPEDWINDOW,
			MAKEINTATOM(s_HelperWindowClass),
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

		if (!s_HelperWindow)
		{
			FLUX_ASSERT(false, "Failed to create helper window.");
			return false;
		}

		ShowWindow(s_HelperWindow, SW_HIDE);

		MSG msg;
		while (PeekMessageW(&msg, s_HelperWindow, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		return true;
	}

	void Platform::Init()
	{
		if (!QueryPerformanceCounter((LARGE_INTEGER*)&s_TimerOffset))
			FLUX_ASSERT(false, "QueryPerformanceCounter failed. ({0})", Platform::GetErrorMessage());
		
		if (!QueryPerformanceFrequency((LARGE_INTEGER*)&s_TimerFrequency))
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

		s_WindowClass = RegisterClassExW(&windowClass);
		if (!s_WindowClass)
			FLUX_ASSERT(false, "RegisterClassExW failed. ({0})", Platform::GetErrorMessage());

		if (!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
			FLUX_ASSERT(false, "Failed to initialize COM library.\n{0}", Platform::GetErrorMessage());

		CreateHelperWindow();
	}

	void Platform::Shutdown()
	{
		DestroyWindow(s_HelperWindow);

		if (!UnregisterClassW(MAKEINTATOM(s_HelperWindowClass), g_Instance))
			FLUX_ASSERT(false, "UnregisterClassExW failed. ({0})", Platform::GetErrorMessage());

		if (!UnregisterClassW(MAKEINTATOM(s_WindowClass), g_Instance))
			FLUX_ASSERT(false, "UnregisterClassExW failed. ({0})", Platform::GetErrorMessage());

		CoUninitialize();
	}

	bool Platform::WaitMessage()
	{
		return ::WaitMessage();
	}

	bool Platform::PostEmptyEvent()
	{
		return ::PostMessageW(s_HelperWindow, WM_NULL, 0, 0);
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
		FLUX_ASSERT(QueryPerformanceCounter((LARGE_INTEGER*)&value));
		return static_cast<float>(value - s_TimerOffset) / static_cast<float>(s_TimerFrequency);
	}

	uint64 Platform::GetNanoTime()
	{
		uint64 value = 0;
		FLUX_ASSERT(QueryPerformanceCounter((LARGE_INTEGER*)&value));
		constexpr uint64 nsPerSecond = 1000 * 1000 * 1000;
		return value * (nsPerSecond / s_TimerFrequency);
	}

	DialogResult Platform::OpenFolderDialog(Window* window, std::string* outPath, const char* title)
	{
		IFileOpenDialog* fileDialog = NULL;

		HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&fileDialog));
		if (!SUCCEEDED(result))
		{
			FLUX_ASSERT(false, "CoCreateInstance failed. ({0})", Platform::GetErrorMessage());
			return DialogResult::None;
		}

		DWORD dwOptions = 0;
		if (!SUCCEEDED(fileDialog->GetOptions(&dwOptions)))
		{
			FLUX_ASSERT(false, "GetOptions for IFileDialog failed.");
			
			fileDialog->Release();
			return DialogResult::None;
		}

		if (!SUCCEEDED(fileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS)))
		{
			FLUX_ASSERT(false, "SetOptions for IFileDialog failed.");
		
			fileDialog->Release();
			return DialogResult::None;
		}

		int32 wTitleSize = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
		wchar_t* wTitle = new wchar_t[wTitleSize];
		MultiByteToWideChar(CP_UTF8, 0, title, -1, wTitle, wTitleSize);
		if (!SUCCEEDED(fileDialog->SetTitle(wTitle)))
		{
			FLUX_ASSERT(false, "SetTitle for IFileDialog failed.");
		
			fileDialog->Release();
			delete[] wTitle;
			return DialogResult::None;
		}
		delete[] wTitle;

		HWND hWnd = static_cast<HWND>(window ? window->GetNativeHandle() : NULL);

		result = fileDialog->Show(hWnd);
		if (SUCCEEDED(result))
		{
			IShellItem* shellItem = NULL;

			result = fileDialog->GetResult(&shellItem);
			if (!SUCCEEDED(result))
			{
				FLUX_ASSERT(false, "GetResult for IFileDialog failed.");

				fileDialog->Release();
				return DialogResult::None;
			}

			wchar_t* path = NULL;
			result = shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &path);
			if (!SUCCEEDED(result))
			{
				FLUX_ASSERT(false, "GetDisplayName for IShellItem failed.");

				shellItem->Release();
				fileDialog->Release();
				return DialogResult::None;
			}

			if (outPath)
			{
				int32 pathSize = WideCharToMultiByte(CP_UTF8, 0, path, -1, NULL, 0, NULL, NULL);
				outPath->resize(pathSize - 1);
				WideCharToMultiByte(CP_UTF8, 0, path, -1, outPath->data(), pathSize - 1, NULL, NULL);
			}

			CoTaskMemFree(path);

			shellItem->Release();
			fileDialog->Release();
			return DialogResult::Ok;
		}
		else if (result == HRESULT_FROM_WIN32(ERROR_CANCELLED))
		{
			fileDialog->Release();
			return DialogResult::Cancel;
		}
		else
		{
			FLUX_ASSERT(false, "Show for IFileDialog failed.");

			fileDialog->Release();
			return DialogResult::None;
		}

		FLUX_ASSERT(false);
		return DialogResult::None;
	}

	DialogResult Platform::MessageBox(MessageBoxButtons buttons, MessageBoxIcon icon, const char* text, const char* caption, Window* window)
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
	
	bool Platform::SetConsoleTitle(const char* title)
	{
		return ::SetConsoleTitleA(title);
	}

	bool Platform::SetThreadName(ThreadHandle handle, const char* name)
	{
		int32 wNameSize = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
		wchar_t* wName = new wchar_t[wNameSize];
		MultiByteToWideChar(CP_UTF8, 0, name, -1, wName, wNameSize);
		bool success = SUCCEEDED(::SetThreadDescription((HANDLE)handle, wName));
		delete[] wName;
		if (!success)
		{
			FLUX_ASSERT(false, "SetThreadDescription failed. ({0})", Platform::GetErrorMessage());
			return false;
		}
		return true;
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

	std::string Platform::GetEnvironmentVariable(const char* variableName)
	{
		static char buffer[65535];
		DWORD bufferSize = ::GetEnvironmentVariableA(variableName, buffer, sizeof(buffer));
		return { buffer, bufferSize };
	}

	bool Platform::SetEnvironmentVariable(const char* variableName, const char* value)
	{
		uint32 error = ::SetEnvironmentVariableA(variableName, value);
		if (error == 0)
		{
			FLUX_WARNING("Failed to set environment variable '{0}' to '{1}'", variableName, value);
			return false;
		}
		return true;
	}

	WindowClassHandle Platform::GetWindowClass()
	{
		return static_cast<WindowClassHandle>(s_WindowClass);
	}

}

#endif