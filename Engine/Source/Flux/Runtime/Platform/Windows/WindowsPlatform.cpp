#include "FluxPCH.h"

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Runtime/Core/Platform.h"

#include "WindowsWindow.h"

#include <ShObjIdl.h>

namespace Flux {

	extern HINSTANCE g_Instance;

	struct PlatformData
	{
		uint64 TimerOffset;
		uint64 TimerFrequency;

		ATOM WindowClass;
	};

	struct MenuData
	{
		WindowMenu Menu = nullptr;
		WindowMenuCallback Callback;
	};

	std::map<Window*, MenuData> g_Menus;

	static PlatformData s_Data;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowsWindow* window = (WindowsWindow*)GetPropW(hWnd, L"Window");
		if (window)
		{
			switch (uMsg)
			{
			case WM_COMMAND:
			{
				auto it = g_Menus.find(window);
				if (it != g_Menus.end())
				{
					auto& data = it->second;
					if (data.Callback)
						data.Callback(data.Menu, (uint32)wParam);
				}
				break;
			}
			case WM_DESTROY:
			{
				auto it = g_Menus.find(window);
				if (it != g_Menus.end())
					g_Menus.erase(it);
				break;
			}
			}

			return window->ProcessMessage(uMsg, wParam, lParam);
		}

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}
	
	void Platform::Init()
	{
		if (!QueryPerformanceCounter((LARGE_INTEGER*)&s_Data.TimerOffset))
			FLUX_ASSERT(false, "QueryPerformanceCounter failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
		
		if (!QueryPerformanceFrequency((LARGE_INTEGER*)&s_Data.TimerFrequency))
			FLUX_ASSERT(false, "QueryPerformanceFrequency failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));

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
			FLUX_ASSERT(false, "RegisterClassExW failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));

		if (!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
			FLUX_ASSERT(false, "Failed to initialize COM library.\n{0}", Platform::GetErrorMessage(Platform::GetLastError()));
	}

	void Platform::Shutdown()
	{
		if (!UnregisterClassW(MAKEINTATOM(s_Data.WindowClass), g_Instance))
			FLUX_ASSERT(false, "UnregisterClassExW failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));

		CoUninitialize();
	}

	bool Platform::WaitMessage()
	{
		return ::WaitMessage();
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
		return static_cast<float>(value - s_Data.TimerOffset) / static_cast<float>(s_Data.TimerFrequency);
	}

	uint64 Platform::GetNanoTime()
	{
		uint64 value = 0;
		FLUX_ASSERT(QueryPerformanceCounter((LARGE_INTEGER*)&value));
		constexpr uint64 nsPerSecond = 1000 * 1000 * 1000;
		return value * (nsPerSecond / s_Data.TimerFrequency);
	}

	WindowMenu Platform::CreateMenu()
	{
		return static_cast<WindowMenu>(::CreateMenu());
	}

	bool Platform::SetMenu(Window* window, WindowMenu menu, WindowMenuCallback callback)
	{
		HWND hWnd = static_cast<HWND>(window ? window->GetNativeHandle() : NULL);
		if (!hWnd)
			return false;

		auto& data = g_Menus[window];
		data.Menu = menu;
		data.Callback = callback;

		FLUX_ERROR("{0} menus ({1})", g_Menus.size(), (void*)window);

		return ::SetMenu(hWnd, static_cast<HMENU>(menu));
	}

	bool Platform::AddMenu(WindowMenu menu, uint32 id, const char* name)
	{
		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_STRING, (UINT_PTR)id, name))
		{
			FLUX_ASSERT(false, "AppendMenuA failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			return false;
		}
		return true;
	}

	bool Platform::AddMenuSeparator(WindowMenu menu)
	{
		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_SEPARATOR | MF_BYPOSITION, NULL, NULL))
		{
			FLUX_ASSERT(false, "AppendMenuA failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			return false;
		}
		return true;
	}

	bool Platform::AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name)
	{
		if (!::AppendMenuA(static_cast<HMENU>(menu), MF_POPUP, (UINT_PTR)childMenu, name))
		{
			FLUX_ASSERT(false, "AppendMenuA failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
			return false;
		}
		return true;
	}

	DialogResult Platform::OpenFolderDialog(Window* window, char** outPath, const char* title)
	{
		IFileOpenDialog* fileDialog = NULL;

		HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&fileDialog));
		if (!SUCCEEDED(result))
		{
			FLUX_ASSERT(false, "CoCreateInstance failed. ({0})", Platform::GetErrorMessage(Platform::GetLastError()));
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

			int32 pathSize = WideCharToMultiByte(CP_UTF8, 0, path, -1, NULL, 0, NULL, NULL);
			*outPath = new char[pathSize];
			(*outPath)[pathSize - 1] = '\0';
			WideCharToMultiByte(CP_UTF8, 0, path, -1, *outPath, pathSize, NULL, NULL);

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

	std::string Platform::GetErrorMessage(int32 error)
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

#define INCLUDE_DOT 0
#if INCLUDE_DOT
		if (message[message.size() - 1] == '.')
			message.pop_back();
#endif

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
		return static_cast<WindowClassHandle>(s_Data.WindowClass);
	}

}

#endif