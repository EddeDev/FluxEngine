#include "FluxPCH.h"
#include "WindowsPlatform.h"

#ifdef FLUX_PLATFORM_WINDOWS

#include "WindowsWindow.h"

namespace Flux {

	static const uint64 s_NSPerSecond = 1000000000;

	static uint64 s_TimerOffset;
	static uint64 s_TimerFrequency;

	void WindowsPlatform::Init()
	{
		FLUX_ASSERT(QueryPerformanceCounter((LARGE_INTEGER*)&s_TimerOffset));
		FLUX_ASSERT(QueryPerformanceFrequency((LARGE_INTEGER*)&s_TimerFrequency));

		DisableProcessWindowsGhosting();

		FLUX_ASSERT(SUCCEEDED(OleInitialize(NULL)), "Failed to initialize the COM library.");
	}

	void WindowsPlatform::Tick()
	{
		MSG msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	void WindowsPlatform::Shutdown()
	{
		OleUninitialize();
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
		return static_cast<float>(value - s_TimerOffset) / s_TimerFrequency;
	}

	uint64 WindowsPlatform::GetNanoTime()
	{
		uint64 value = 0;
		FLUX_ASSERT(QueryPerformanceCounter((LARGE_INTEGER*)&value));
		return value * (s_NSPerSecond / s_TimerFrequency);
	}

	MessageBoxResult WindowsPlatform::MessageBox(Window* window, MessageBoxButtons buttons, MessageBoxIcon icon, const char* text, const char* caption)
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

		int32 result = MessageBoxA(hWnd, text, caption, flags);
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

}

#endif