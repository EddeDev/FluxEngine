#include "FluxPCH.h"

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Runtime/Core/Platform.h"

#include "WindowsWindow.h"

#include <ShObjIdl.h>

namespace Flux {

	namespace Utils {

		static IntRect IntRectFromWin32Rect(RECT rect)
		{
			IntRect result;
			result.MinX = rect.left;
			result.MinY = rect.top;
			result.MaxX = rect.right;
			result.MaxY = rect.bottom;
			return result;
		}

	}

	extern HINSTANCE g_Instance;

	struct WindowsPlatformData
	{
		uint64 TimerOffset;
		uint64 TimerFrequency;

		ATOM WindowClass;

		ATOM HelperWindowClass;
		HWND HelperWindowHandle;

		int16 KeyCodes[512];
		int16 ScanCodes[FLUX_KEY_LAST + 1];
		char KeyNames[FLUX_KEY_LAST + 1][5];
	};

	static WindowsPlatformData* s_Data = nullptr;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowsWindow* window = (WindowsWindow*)GetPropW(hWnd, L"Window");
		if (window)
			return window->ProcessMessage(uMsg, wParam, lParam);

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	static bool CreateHelperWindow()
	{
		WNDCLASSEXW wc = {};
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = DefWindowProcW;
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

	static void CreateKeyTables()
	{
		// From glfw (win32_init.c)
		s_Data->KeyCodes[0x00B] = FLUX_KEY_0;
		s_Data->KeyCodes[0x002] = FLUX_KEY_1;
		s_Data->KeyCodes[0x003] = FLUX_KEY_2;
		s_Data->KeyCodes[0x004] = FLUX_KEY_3;
		s_Data->KeyCodes[0x005] = FLUX_KEY_4;
		s_Data->KeyCodes[0x006] = FLUX_KEY_5;
		s_Data->KeyCodes[0x007] = FLUX_KEY_6;
		s_Data->KeyCodes[0x008] = FLUX_KEY_7;
		s_Data->KeyCodes[0x009] = FLUX_KEY_8;
		s_Data->KeyCodes[0x00A] = FLUX_KEY_9;
		s_Data->KeyCodes[0x01E] = FLUX_KEY_A;
		s_Data->KeyCodes[0x030] = FLUX_KEY_B;
		s_Data->KeyCodes[0x02E] = FLUX_KEY_C;
		s_Data->KeyCodes[0x020] = FLUX_KEY_D;
		s_Data->KeyCodes[0x012] = FLUX_KEY_E;
		s_Data->KeyCodes[0x021] = FLUX_KEY_F;
		s_Data->KeyCodes[0x022] = FLUX_KEY_G;
		s_Data->KeyCodes[0x023] = FLUX_KEY_H;
		s_Data->KeyCodes[0x017] = FLUX_KEY_I;
		s_Data->KeyCodes[0x024] = FLUX_KEY_J;
		s_Data->KeyCodes[0x025] = FLUX_KEY_K;
		s_Data->KeyCodes[0x026] = FLUX_KEY_L;
		s_Data->KeyCodes[0x032] = FLUX_KEY_M;
		s_Data->KeyCodes[0x031] = FLUX_KEY_N;
		s_Data->KeyCodes[0x018] = FLUX_KEY_O;
		s_Data->KeyCodes[0x019] = FLUX_KEY_P;
		s_Data->KeyCodes[0x010] = FLUX_KEY_Q;
		s_Data->KeyCodes[0x013] = FLUX_KEY_R;
		s_Data->KeyCodes[0x01F] = FLUX_KEY_S;
		s_Data->KeyCodes[0x014] = FLUX_KEY_T;
		s_Data->KeyCodes[0x016] = FLUX_KEY_U;
		s_Data->KeyCodes[0x02F] = FLUX_KEY_V;
		s_Data->KeyCodes[0x011] = FLUX_KEY_W;
		s_Data->KeyCodes[0x02D] = FLUX_KEY_X;
		s_Data->KeyCodes[0x015] = FLUX_KEY_Y;
		s_Data->KeyCodes[0x02C] = FLUX_KEY_Z;
		s_Data->KeyCodes[0x028] = FLUX_KEY_APOSTROPHE;
		s_Data->KeyCodes[0x02B] = FLUX_KEY_BACKSLASH;
		s_Data->KeyCodes[0x033] = FLUX_KEY_COMMA;
		s_Data->KeyCodes[0x00D] = FLUX_KEY_EQUAL;
		s_Data->KeyCodes[0x029] = FLUX_KEY_GRAVE_ACCENT;
		s_Data->KeyCodes[0x01A] = FLUX_KEY_LEFT_BRACKET;
		s_Data->KeyCodes[0x00C] = FLUX_KEY_MINUS;
		s_Data->KeyCodes[0x034] = FLUX_KEY_PERIOD;
		s_Data->KeyCodes[0x01B] = FLUX_KEY_RIGHT_BRACKET;
		s_Data->KeyCodes[0x027] = FLUX_KEY_SEMICOLON;
		s_Data->KeyCodes[0x035] = FLUX_KEY_SLASH;
		s_Data->KeyCodes[0x056] = FLUX_KEY_WORLD_2;
		s_Data->KeyCodes[0x00E] = FLUX_KEY_BACKSPACE;
		s_Data->KeyCodes[0x153] = FLUX_KEY_DELETE;
		s_Data->KeyCodes[0x14F] = FLUX_KEY_END;
		s_Data->KeyCodes[0x01C] = FLUX_KEY_ENTER;
		s_Data->KeyCodes[0x001] = FLUX_KEY_ESCAPE;
		s_Data->KeyCodes[0x147] = FLUX_KEY_HOME;
		s_Data->KeyCodes[0x152] = FLUX_KEY_INSERT;
		s_Data->KeyCodes[0x15D] = FLUX_KEY_MENU;
		s_Data->KeyCodes[0x151] = FLUX_KEY_PAGE_DOWN;
		s_Data->KeyCodes[0x149] = FLUX_KEY_PAGE_UP;
		s_Data->KeyCodes[0x045] = FLUX_KEY_PAUSE;
		s_Data->KeyCodes[0x039] = FLUX_KEY_SPACE;
		s_Data->KeyCodes[0x00F] = FLUX_KEY_TAB;
		s_Data->KeyCodes[0x03A] = FLUX_KEY_CAPS_LOCK;
		s_Data->KeyCodes[0x145] = FLUX_KEY_NUM_LOCK;
		s_Data->KeyCodes[0x046] = FLUX_KEY_SCROLL_LOCK;
		s_Data->KeyCodes[0x03B] = FLUX_KEY_F1;
		s_Data->KeyCodes[0x03C] = FLUX_KEY_F2;
		s_Data->KeyCodes[0x03D] = FLUX_KEY_F3;
		s_Data->KeyCodes[0x03E] = FLUX_KEY_F4;
		s_Data->KeyCodes[0x03F] = FLUX_KEY_F5;
		s_Data->KeyCodes[0x040] = FLUX_KEY_F6;
		s_Data->KeyCodes[0x041] = FLUX_KEY_F7;
		s_Data->KeyCodes[0x042] = FLUX_KEY_F8;
		s_Data->KeyCodes[0x043] = FLUX_KEY_F9;
		s_Data->KeyCodes[0x044] = FLUX_KEY_F10;
		s_Data->KeyCodes[0x057] = FLUX_KEY_F11;
		s_Data->KeyCodes[0x058] = FLUX_KEY_F12;
		s_Data->KeyCodes[0x064] = FLUX_KEY_F13;
		s_Data->KeyCodes[0x065] = FLUX_KEY_F14;
		s_Data->KeyCodes[0x066] = FLUX_KEY_F15;
		s_Data->KeyCodes[0x067] = FLUX_KEY_F16;
		s_Data->KeyCodes[0x068] = FLUX_KEY_F17;
		s_Data->KeyCodes[0x069] = FLUX_KEY_F18;
		s_Data->KeyCodes[0x06A] = FLUX_KEY_F19;
		s_Data->KeyCodes[0x06B] = FLUX_KEY_F20;
		s_Data->KeyCodes[0x06C] = FLUX_KEY_F21;
		s_Data->KeyCodes[0x06D] = FLUX_KEY_F22;
		s_Data->KeyCodes[0x06E] = FLUX_KEY_F23;
		s_Data->KeyCodes[0x076] = FLUX_KEY_F24;
		s_Data->KeyCodes[0x038] = FLUX_KEY_LEFT_ALT;
		s_Data->KeyCodes[0x01D] = FLUX_KEY_LEFT_CONTROL;
		s_Data->KeyCodes[0x02A] = FLUX_KEY_LEFT_SHIFT;
		s_Data->KeyCodes[0x15B] = FLUX_KEY_LEFT_SUPER;
		s_Data->KeyCodes[0x137] = FLUX_KEY_PRINT_SCREEN;
		s_Data->KeyCodes[0x138] = FLUX_KEY_RIGHT_ALT;
		s_Data->KeyCodes[0x11D] = FLUX_KEY_RIGHT_CONTROL;
		s_Data->KeyCodes[0x036] = FLUX_KEY_RIGHT_SHIFT;
		s_Data->KeyCodes[0x15C] = FLUX_KEY_RIGHT_SUPER;
		s_Data->KeyCodes[0x150] = FLUX_KEY_DOWN;
		s_Data->KeyCodes[0x14B] = FLUX_KEY_LEFT;
		s_Data->KeyCodes[0x14D] = FLUX_KEY_RIGHT;
		s_Data->KeyCodes[0x148] = FLUX_KEY_UP;
		s_Data->KeyCodes[0x052] = FLUX_KEY_KP_0;
		s_Data->KeyCodes[0x04F] = FLUX_KEY_KP_1;
		s_Data->KeyCodes[0x050] = FLUX_KEY_KP_2;
		s_Data->KeyCodes[0x051] = FLUX_KEY_KP_3;
		s_Data->KeyCodes[0x04B] = FLUX_KEY_KP_4;
		s_Data->KeyCodes[0x04C] = FLUX_KEY_KP_5;
		s_Data->KeyCodes[0x04D] = FLUX_KEY_KP_6;
		s_Data->KeyCodes[0x047] = FLUX_KEY_KP_7;
		s_Data->KeyCodes[0x048] = FLUX_KEY_KP_8;
		s_Data->KeyCodes[0x049] = FLUX_KEY_KP_9;
		s_Data->KeyCodes[0x04E] = FLUX_KEY_KP_ADD;
		s_Data->KeyCodes[0x053] = FLUX_KEY_KP_DECIMAL;
		s_Data->KeyCodes[0x135] = FLUX_KEY_KP_DIVIDE;
		s_Data->KeyCodes[0x11C] = FLUX_KEY_KP_ENTER;
		s_Data->KeyCodes[0x059] = FLUX_KEY_KP_EQUAL;
		s_Data->KeyCodes[0x037] = FLUX_KEY_KP_MULTIPLY;
		s_Data->KeyCodes[0x04A] = FLUX_KEY_KP_SUBTRACT;

		for (int32 scancode = 0; scancode < 512; scancode++)
		{
			if (s_Data->KeyCodes[scancode] > 0)
				s_Data->ScanCodes[s_Data->KeyCodes[scancode]] = scancode;
		}
	}

	static void UpdateKeyNames()
	{
		const uint32 virtualKeys[] = {
			VK_NUMPAD0,  VK_NUMPAD1,  VK_NUMPAD2, VK_NUMPAD3,
			VK_NUMPAD4,  VK_NUMPAD5,  VK_NUMPAD6, VK_NUMPAD7,
			VK_NUMPAD8,  VK_NUMPAD9,  VK_DECIMAL, VK_DIVIDE,
			VK_MULTIPLY, VK_SUBTRACT, VK_ADD
		};

		uint8 state[256] = { 0 };

		for (int32 key = FLUX_KEY_SPACE; key <= FLUX_KEY_LAST; key++)
		{
			int32 scancode = s_Data->ScanCodes[key];
			if (scancode == -1)
				continue;

			uint32 virtualKey;
			if (key >= FLUX_KEY_KP_0 && key <= FLUX_KEY_KP_ADD)
				virtualKey = virtualKeys[key - FLUX_KEY_KP_0];
			else
				virtualKey = MapVirtualKeyW(scancode, MAPVK_VSC_TO_VK);

			wchar_t chars[16];

			int32 length = ToUnicode(virtualKey, scancode, state, chars, sizeof(chars) / sizeof(wchar_t), 0);
			if (length == -1)
				length = ToUnicode(virtualKey, scancode, state, chars, sizeof(chars) / sizeof(wchar_t), 0);

			if (length < -1)
				continue;

			WideCharToMultiByte(CP_UTF8, 0, chars, 1, s_Data->KeyNames[key], sizeof(s_Data->KeyNames[key]), NULL, NULL);
		}
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

		CreateKeyTables();
		UpdateKeyNames();

		CreateHelperWindow();

#if 0
		UpdateMonitors();
#endif
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

	DialogResult Platform::OpenFolderDialog(Window* window, std::string* outPath, const std::string& title)
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

		wchar_t* titleBuffer = new wchar_t[title.size() + 1];
		MultiByteToWideChar(CP_UTF8, 0, title.data(), -1, titleBuffer, static_cast<int32>(title.size()) + 1);
		if (!SUCCEEDED(fileDialog->SetTitle(titleBuffer)))
		{
			FLUX_ASSERT(false, "SetTitle for IFileDialog failed.");

			fileDialog->Release();
			delete[] titleBuffer;
			return DialogResult::None;
		}
		delete[] titleBuffer;

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
		MultiByteToWideChar(CP_UTF8, 0, name.data(), -1, buffer, static_cast<int32>(name.size()) + 1);
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

	MonitorHandle Platform::GetPrimaryMonitorHandle()
	{
		return MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
	}

	MonitorInfo Platform::GetMonitorInfo(MonitorHandle handle)
	{
		MONITORINFO mi = {};
		mi.cbSize = sizeof(MONITORINFO);
		if (!::GetMonitorInfo((HMONITOR)handle, &mi))
			FLUX_ASSERT(false, "GetMonitorInfo failed. ({0})", Platform::GetErrorMessage());

		MonitorInfo result = {};
		result.Rect = Utils::IntRectFromWin32Rect(mi.rcMonitor);
		result.WorkRect = Utils::IntRectFromWin32Rect(mi.rcWork);
		return result;
	}

	MonitorHandleList Platform::GetMonitorHandles()
	{
		MonitorHandleList handles;
		::EnumDisplayMonitors(NULL, NULL, [](HMONITOR handle, HDC hDC, RECT* rect, LPARAM data)
		{
			auto handles = (MonitorHandleList*)data;
			handles->push_back(handle);
			return TRUE;
		}, (LPARAM)&handles);
		return handles;
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

	int16 Platform::GetKeyCode(int32 scancode)
	{
		return s_Data->KeyCodes[scancode];
	}

	int16 Platform::GetScanCode(int16 key)
	{
		return s_Data->ScanCodes[key];
	}

	char* Platform::GetKeyName(int32 key)
	{
		return s_Data->KeyNames[key];
	}

}

#endif